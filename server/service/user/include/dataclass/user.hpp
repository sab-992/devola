#pragma once

#include <core/time.hpp>
#include <core/str.hpp>
#include <database/utility/timestamp.hpp>
#include <database/value.hpp>
#include <chrono>
#include <nlohmann/json.hpp>


struct User {
    std::string uuid;
    std::string username;
    std::string email;
    std::string first_name;
    std::string last_name;

    static User fromDatabaseFormat(const database_n::record_t& record);
    static User fromJSON(const nlohmann::json& object);
    static std::vector<std::string> projection();
    nlohmann::json toJSON() const;
};

struct UserDBInformation {
    User user;
    bool is_active;
    bool is_email_verified;
    std::chrono::time_point<std::chrono::system_clock> created_at;
    std::chrono::time_point<std::chrono::system_clock> last_updated_at;
    std::chrono::time_point<std::chrono::system_clock> last_login;

    static UserDBInformation fromDatabaseFormat(const database_n::record_t& record);
    static UserDBInformation fromJSON(const nlohmann::json& object);
    nlohmann::json toJSON() const;

private:
    static std::chrono::time_point<std::chrono::system_clock> format(const std::string& format, std::string_view value);
};