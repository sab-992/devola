#include <service/user.hpp>


UserService::UserService(const Private_s&, const json& configJSON) : m_configJSON(configJSON), Basic("UserService", configJSON["server"]["port"]) {
    setStartSequence([&](Basic*){
        assert(this->m_database && "[UserService]: No database given");
        m_userRepos = std::make_unique<UserRepository>(tools());
    });

    setEndpoints();
}

UserService::~UserService() {}

nlohmann::json UserService::validateJWT(const http_n::Request& request) const {
    const std::string& token = JWT::getToken(request);
    // TODO move inside private function that will actually check if the JWT is banned/revoked and verify
    return JWT::verify(token);
}

asio::awaitable<http_n::Response> UserService::authenticate(const Session& session, const http_n::Request& request, const pathParams_t&) {
    try {
        validateJWT(request);
        co_return Response().setStatus(Code::OK).build();
    } catch (const Exception& e) {
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

        JWT::generate(user->toJSON(), response);

        co_return response.setStatus(Code::OK).build();
    } catch (const Exception& e) {
        co_return Response().setStatus(e.code()).build();
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, "while signing user in:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

asio::awaitable<http_n::Response> UserService::logout(const Session& session, const http_n::Request& request, const pathParams_t&) {
    try {
        const json& userInfo = validateJWT(request);
        // TODO
        co_return Response().setStatus(Code::OK).build();
    } catch (const Exception& e) {
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
        const json& userInfo = validateJWT(request);
        // TODO
        co_return Response().setStatus(Code::OK).build();
    } catch (const Exception& e) {
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
    } catch (const Exception& e) {
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
    ENDPOINT("POST", "/logout",       &UserService::logout);
    ENDPOINT("POST", "/refresh",      &UserService::refresh);
    ENDPOINT("POST", "/register",     &UserService::register_);
}

user::ServerTools UserService::tools() {
    return { m_database };
}