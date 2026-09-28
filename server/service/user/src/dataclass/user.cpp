#include <dataclass/user.hpp>


User User::fromDatabaseFormat(const database_n::record_t& record) {
    return { record.at("uuid").asString(),
             record.at("username").asString(),
             record.at("email").asString(),
             record.at("first_name").asString(),
             record.at("last_name").asString() };
}

User User::fromJSON(const nlohmann::json& object) {
    return { object["uuid"].get<std::string>(),
             object["username"].get<std::string>(),
             object["email"].get<std::string>(),
             object["first_name"].get<std::string>(),
             object["last_name"].get<std::string>() };
}

nlohmann::json User::toJSON() const {
    return nlohmann::json({ { "uuid",       uuid },
                            { "username",   username },
                            { "email",      email },
                            { "first_name", first_name },
                            { "last_name",  last_name }});
}

std::vector<std::string> User::projection() {
    return { "uuid", "username", "email", "first_name", "last_name" };
}

UserDBInformation UserDBInformation::fromDatabaseFormat(const database_n::record_t& record) {
    return { User::fromDatabaseFormat(record),
             record.at("is_active").asBool(),
             record.at("is_email_verified").asBool(),
             fromPGSQLFormat(record.at("created_at").asString()),
             fromPGSQLFormat(record.at("last_updated_at").asString()),
             fromPGSQLFormat(record.at("last_login").asString()) };
}

UserDBInformation UserDBInformation::fromJSON(const nlohmann::json& object) {
    return { User::fromJSON(object["user"]),
             object["is_active"].get<bool>(),
             object["is_email_verified"].get<bool>(),
             Time::fromSecondSinceEpoch(object["created_at"].get<double>()),
             Time::fromSecondSinceEpoch(object["last_updated_at"].get<double>()),
             Time::fromSecondSinceEpoch(object["last_login"].get<double>()) };
}

nlohmann::json UserDBInformation::toJSON() const {
    return nlohmann::json({ { "user",              user.toJSON() },
                            { "is_active",         is_active },
                            { "is_email_verified", is_email_verified },
                            { "created_at",        Time::toSecondsSinceEpoch(created_at) },
                            { "last_updated_at",        Time::toSecondsSinceEpoch(last_updated_at) },
                            { "last_login",        Time::toSecondsSinceEpoch(last_login) }});
}