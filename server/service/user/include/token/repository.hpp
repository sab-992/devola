#pragma once

#include <core/http.hpp>
#include <core/jwt.hpp>
#include <core/str.hpp>
#include <core/utility.hpp>
#include <dataclass/refresh_token.hpp>
#include <database/postgres.hpp>
#include <database/utility/timestamp.hpp>
#include <service/tools.hpp>


class TokenRepository {
    using Basic = http_n::server_n::Basic;
    using Session = http_n::server_n::Session;
    using json = nlohmann::json;
    using Database_i = database_n::Database_i;
    using ServerTools = user::ServerTools;
    using record_t = database_n::record_t;

public:
    TokenRepository(const ServerTools& tools);
    ~TokenRepository() = default;

    friend std::unique_ptr<TokenRepository> std::make_unique<TokenRepository>();

    void createRefreshToken(const RefreshToken& token, std::string_view userUUID);
    std::string validateRefreshToken(std::string_view token);
    void revokeRefreshToken(std::string_view token);

private:
    std::shared_ptr<Cryptography> m_crypto = Cryptography::instance();
    ServerTools m_tools;

    inline static std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();

    std::shared_ptr<Database_i> database();
};