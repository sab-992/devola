#pragma once

#include <core/http.hpp>
#include <core/file.hpp>
#include <core/process.hpp>
#include <core/utility.hpp>
#include <core/jwt.hpp>
#include <database/postgres.hpp>
#include <database/redis.hpp>
#include <token/repository.hpp>
#include <user/repository.hpp>
#include <service/tools.hpp>


class UserService : public http_n::server_n::Basic, public Service<UserService> {
    using Basic = http_n::server_n::Basic;
    using Code = network_n::Code;
    using Database_i = database_n::Database_i;
    using json = nlohmann::json;
    using Response = http_n::Response;
    using Session = http_n::server_n::Session;

public:
    UserService(const Private_s&, const json& configJSON);
    ~UserService();

    std::string pathPrefix() const override;
    void setDatabase(std::shared_ptr<Database_i> database);
    void setRevokedTokenCache(std::shared_ptr<Database_i> database);

    static std::unique_ptr<UserService> create(const json& configJSON);

private:
    json m_configJSON;
    std::shared_ptr<Database_i> m_database;
    std::shared_ptr<Database_i> m_revokedTokenCache;
    std::unique_ptr<TokenRepository> m_tokenRepos;
    std::unique_ptr<UserRepository> m_userRepos;

    asio::awaitable<Response> authenticate(const Session& session, const http_n::Request& request, const pathParams_t&);
    asio::awaitable<Response> login(const Session& session, const http_n::Request& request, const pathParams_t&);
    asio::awaitable<Response> logout(const Session& session, const http_n::Request& request, const pathParams_t&);
    asio::awaitable<Response> refresh(const Session& session, const http_n::Request& request, const pathParams_t&);
    asio::awaitable<Response> register_(const Session& session, const http_n::Request& request, const pathParams_t&);

    void setEndpoints();
    user::ServerTools tools();
    json validateJWT(const http_n::Request& request) const;
    bool validateUserIdentity(const http_n::Request& request, json& claims) const;
};