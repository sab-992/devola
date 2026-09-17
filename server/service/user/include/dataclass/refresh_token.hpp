#pragma once

#include <core/time.hpp>
#include <core/str.hpp>
#include <database/utility/timestamp.hpp>
#include <database/value.hpp>
#include <chrono>
#include <nlohmann/json.hpp>


struct RefreshToken {
    std::string token; // Hashed token.
    std::string user_uuid;
    std::chrono::time_point<std::chrono::system_clock> created_at;
    std::chrono::time_point<std::chrono::system_clock> expire_at;
    bool revoked;

    static RefreshToken fromDatabaseFormat(const database_n::record_t& record);
    static std::vector<std::string> projection();
    database_n::record_t toDatabaseFormat() const;
    nlohmann::json toJSON() const;
};