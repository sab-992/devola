#include <dataclass/recommendation.hpp>


RecommendationTask RecommendationTask::fromJSON(const nlohmann::json& object) {
    return { object["uuid"].get<std::string>(),
             object["status"].get<std::string>(),
             Time::fromSecondSinceEpoch(object["started_at"].get<double>()),
             Time::fromSecondSinceEpoch(object["last_updated_at"].get<double>()) };
}

RecommendationTask RecommendationTask::fromDatabaseFormat(const database_n::record_t& record) {
    return { record.at("uuid").asString(),
             record.at("status").asString(),
             fromPGSQLFormat(record.at("started_at").asString()),
             fromPGSQLFormat(record.at("last_updated_at").asString()) };
}

std::vector<std::string> RecommendationTask::projection() {
    return { "uuid", "status", "started_at", "last_updated_at" };
}

nlohmann::json RecommendationTask::toJSON() const {
    auto object = nlohmann::json({ { "uuid",             uuid },
                                   { "status",           status },
                                   { "started_at",       Time::toSecondsSinceEpoch(started_at) },
                                   { "last_updated_at",  Time::toSecondsSinceEpoch(last_updated_at) }});

    return object;
}