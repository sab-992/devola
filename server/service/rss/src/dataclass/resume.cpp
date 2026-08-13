#include <dataclass/resume.hpp>


Resume Resume::fromJSON(const nlohmann::json& object) {
    return { object["tag"].get<std::string>(),
             object["content"].get<std::string>(),
             Time::fromSecondSinceEpoch(object["created_at"].get<double>()),
             Time::fromSecondSinceEpoch(object["last_updated_at"].get<double>()) };
}

Resume Resume::fromDatabaseFormat(const database_n::record_t& record) {
    return { record.at("tag").asString(),
             record.at("content").asString(),
             fromPGSQLFormat(record.at("created_at").asString()),
             fromPGSQLFormat(record.at("last_updated_at").asString()) };
}

std::vector<std::string> Resume::projection() {
    return { "tag", "content", "created_at", "last_updated_at" };
}

nlohmann::json Resume::toJSON() const {
    auto object = nlohmann::json({ { "tag",     tag },
                                   { "content", content },
                                   { "created_at",      Time::toSecondsSinceEpoch(created_at) },
                                   { "last_updated_at", Time::toSecondsSinceEpoch(created_at) }});

    return object;
}