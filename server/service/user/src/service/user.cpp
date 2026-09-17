#include <service/user.hpp>


UserService::UserService(const Private_s&, const json& configJSON) : m_configJSON(configJSON), Basic("UserService", configJSON["server"]["port"]) {
    setStartSequence([&](Basic*){
        assert(this->m_database          && "[UserService]: No database given");
        assert(this->m_revokedTokenCache && "[UserService]: No revoked token cache given");
        m_tokenRepos = std::make_unique<TokenRepository>(tools());
        m_userRepos = std::make_unique<UserRepository>(tools());
    });

    setEndpoints();
}

UserService::~UserService() {}

nlohmann::json UserService::validateJWT(const http_n::Request& request) const {
    using namespace database_n;
    const std::string& token = JWT::getToken(request);

    if (trim(token).empty())
        throw InvalidToken(std::format("Token is empty"));

    const Result& result = this->m_revokedTokenCache->Read(Query().setTarget(token)
                                                                  .setType(Query::Type_en::TARGETED)
                                                                  .setCardinality(Query::Cardinality_en::SINGLE).build());
    if (not result.isOK())
        throw Exception(std::format("Revoked token cache returned error while validating JWT: {}", result.error().value()));

    if (result.records().has_value() and not result.records().value()[0].at(token).isNull())
        throw InvalidToken(std::format("Token \"{}...\" is revoked", token.substr(0, 15)));

    return JWT::verify(token);
}

asio::awaitable<http_n::Response> UserService::authenticate(const Session& session, const http_n::Request& request, const pathParams_t&) {
    try {
        validateJWT(request);
        co_return Response().setStatus(Code::OK).build();
    } catch (const LogicException& e) {
        co_return Response().setStatus(e.code()).build();
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, "while authenticating user:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

std::unique_ptr<UserService> UserService::create(const json& configJSON) {
    return std::make_unique<UserService>(Private_s(), configJSON);
}

asio::awaitable<http_n::Response> UserService::login(const Session& session, const http_n::Request& request, const pathParams_t&) {
    try {
        const auto& body = request.body<nlohmann::json>();
        std::unique_ptr<User> user = m_userRepos->fetchUser(body["username"].get<std::string>(),
                                                            body["password"].get<std::string>());

        Response response;
        if (not user)
            co_return response.setStatus(Code::NOT_FOUND).build();

        JWT::generateJWT(user->toJSON(), response);
        const auto& now = Time::now();
        m_tokenRepos->createRefreshToken({ JWT::generateRefreshToken(response), user->uuid, now, now + JWT::REFRESH_TTL, false }, user->uuid);

        co_return response.setStatus(Code::OK).build();
    } catch (const LogicException& e) {
        co_return Response().setStatus(e.code()).build();
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, "while signing user in:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

asio::awaitable<http_n::Response> UserService::logout(const Session& session, const http_n::Request& request, const pathParams_t&) {
    using namespace database_n;

    try {
        const std::string& token = JWT::getToken(request);
        if (trim(token).empty())
            throw InvalidArgument("No JWT provided");

        const std::string& refreshToken = JWT::getRefreshToken(request);
        if (trim(refreshToken).empty())
            throw InvalidArgument("No refresh token provided");

        const json& claims = JWT::verify(token);

        Query::Options opt;
        opt.ttl = std::chrono::duration_cast<std::chrono::seconds>(Time::fromSecondSinceEpoch(claims["exp"].get<double>()) - Time::now());
        m_revokedTokenCache->Create(Query().setTarget(token)
                                           .setType(Query::Type_en::TARGETED)
                                           .setCardinality(Query::Cardinality_en::NONE)
                                           .setData({ { token, "" }})
                                           .setOptions(opt).build());

        m_tokenRepos->revokeRefreshToken(refreshToken);
        auto response = Response();
        JWT::clearBrowserToken(response);
        co_return response.setStatus(Code::OK).build();
    } catch (const LogicException& e) {
        co_return Response().setStatus(e.code()).build();
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, "while signing user out:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

std::string UserService::pathPrefix() const {
    return "/user";
}

asio::awaitable<http_n::Response> UserService::refresh(const Session& session, const http_n::Request& request, const pathParams_t&) {
    try {
        const std::string& userUUID = m_tokenRepos->validateRefreshToken(JWT::getRefreshToken(request));
        const std::unique_ptr<User> user = m_userRepos->fetchUserByUUID(userUUID);

        auto response = Response();
        JWT::generateJWT(user->toJSON(), response);
        co_return response.setStatus(Code::OK).build();
    } catch (const LogicException& e) {
        co_return Response().setStatus(e.code()).build();
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, "while refreshing user's JWT:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

asio::awaitable<http_n::Response> UserService::register_(const Session& session, const http_n::Request& request, const pathParams_t&) {
    try {
        m_userRepos->createUser(request.body<nlohmann::json>());
        co_return Response().setStatus(Code::CREATED).build();
    } catch (const LogicException& e) {
        co_return Response().setStatus(e.code()).build();
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, "while creating user:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

void UserService::setDatabase(std::shared_ptr<Database_i> database) {
    m_database = database;
}

void UserService::setEndpoints() {
    ENDPOINT("POST", "/authenticate", &UserService::authenticate);
    ENDPOINT("POST", "/login",        &UserService::login);
    ENDPOINT("POST", "/auth/logout",  &UserService::logout);
    ENDPOINT("POST", "/auth/refresh", &UserService::refresh);
    ENDPOINT("POST", "/register",     &UserService::register_);
}

void UserService::setRevokedTokenCache(std::shared_ptr<Database_i> database) {
    m_revokedTokenCache = database;
}

user::ServerTools UserService::tools() {
    return { m_database };
}