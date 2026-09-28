#include <dataclass/refresh_token.hpp>


RefreshToken RefreshToken::fromDatabaseFormat(const database_n::record_t& record) {
    return { record.at("token").asString(),
             record.at("user_uuid").asString(),
             fromPGSQLFormat(record.at("created_at").asString()),
             fromPGSQLFormat(record.at("expire_at").asString()),
             record.at("revoked").asBool() };
}

std::vector<std::string> RefreshToken::projection() {
    return { "token", "user_uuid", "created_at", "expire_at", "revoked" };
}

database_n::record_t RefreshToken::toDatabaseFormat() const {
    return {{ "token",      token },
            { "user_uuid",  user_uuid },
            { "created_at", toPGSQLFormat(created_at) },
            { "expire_at",  toPGSQLFormat(expire_at) },
            { "revoked",    revoked }};
}

nlohmann::json RefreshToken::toJSON() const {
    return nlohmann::json({ { "token",      token },
                            { "user_uuid",  user_uuid },
                            { "created_at", Time::toSecondsSinceEpoch(created_at) },
                            { "expire_at",  Time::toSecondsSinceEpoch(expire_at) },
                            { "revoked",    revoked }});
}