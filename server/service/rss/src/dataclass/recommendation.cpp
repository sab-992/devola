#include <dataclass/recommendation.hpp>


Recommendation Recommendation::fromJSON(const nlohmann::json& object) {
    return { object["task_uuid"].get<std::string>(),
             object["status"].get<std::string>(),
             object["website_host"].get<std::string>(),
             object["website_endpoint"].get<std::string>(),
             Time::fromSecondSinceEpoch(object["started_at"].get<double>()),
             Time::fromSecondSinceEpoch(object["last_updated_at"].get<double>()) };
}

Recommendation Recommendation::fromDatabaseFormat(const database_n::record_t& record) {
    return { record.at("task_uuid").asString(),
             record.at("status").asString(),
             record.at("website_host").asString(),
             record.at("website_endpoint").asString(),
             fromPGSQLFormat(record.at("started_at").asString()),
             fromPGSQLFormat(record.at("last_updated_at").asString()) };
}

std::vector<std::string> Recommendation::projection() {
    return { "task_uuid", "status", "website_host", "website_endpoint", "started_at", "last_updated_at" };
}

nlohmann::json Recommendation::toJSON() const {
    auto object = nlohmann::json({ { "task_uuid",        task_uuid },
                                   { "status",           status },
                                   { "website_host",     website_host },
                                   { "website_endpoint", website_endpoint },
                                   { "started_at",       Time::toSecondsSinceEpoch(std::chrono::floor<std::chrono::seconds>(started_at)) },
                                   { "last_updated_at",  Time::toSecondsSinceEpoch(std::chrono::floor<std::chrono::seconds>(last_updated_at)) }});

    return object;
}