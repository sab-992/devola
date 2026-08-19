#include <service/user.hpp>


UserService::UserService(const Private_s&, const json& configJSON) : m_configJSON(configJSON), Basic("UserService", configJSON["server"]["port"]) {
    setStartSequence([&](Basic*){
        assert(this->m_database && "[UserService]: No database given");
        m_userRepos = std::make_unique<UserRepository>(tools());
    });

    setEndpoints();
}

UserService::~UserService() {}

asio::awaitable<http_n::Response> UserService::authenticate(const Session& session, const http_n::Request& request, const pathParams_t&) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    co_return Response().setStatus(Code::OK).build();
}

std::unique_ptr<UserService> UserService::create(const json& configJSON) {
    return std::make_unique<UserService>(Private_s(), configJSON);
}

asio::awaitable<http_n::Response> UserService::login(const Session& session, const http_n::Request& request, const pathParams_t&) {
    const auto& body = request.body<nlohmann::json>();
    std::unique_ptr<User> user = m_userRepos->fetchUser(body["username"].get<std::string>(),
                                                        body["password"].get<std::string>());
    Response response;
    if (not user)
        co_return response.setStatus(Code::NOT_FOUND).build();

    const std::string& jwt = JWT::generate(user->toJSON());
    JWT::setToken(response, jwt);

    co_return response.setStatus(Code::OK).build();
}

asio::awaitable<http_n::Response> UserService::logout(const Session& session, const http_n::Request& request, const pathParams_t&) {
    // TODO
    co_return Response().build();
}

std::string UserService::pathPrefix() const {
    return "/user";
}

asio::awaitable<http_n::Response> UserService::refresh(const Session& session, const http_n::Request& request, const pathParams_t&) {
    // TODO
    co_return Response().build();
}

asio::awaitable<http_n::Response> UserService::register_(const Session& session, const http_n::Request& request, const pathParams_t&) {
    m_userRepos->createUser(request.body<nlohmann::json>());
    co_return Response().setStatus(Code::CREATED).build();
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

bool UserService::validateUserIdentity(const http_n::Request& request, json& claims) const {
    try {
        claims = JWT::verify(JWT::getToken(request));
    } catch (const std::exception& e) {
        return false;
    }
    return true;
}