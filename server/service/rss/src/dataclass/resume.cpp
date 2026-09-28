#include <dataclass/resume.hpp>


Resume Resume::fromJSON(const nlohmann::json& object) {
    return { object["tag"].get<std::string>(),
             object["content"].get<std::string>(),
             object["skills"].get<std::vector<std::string>>(),
             Time::fromSecondSinceEpoch(object["last_updated_at"].get<double>()) };
}

Resume Resume::fromDatabaseFormat(const database_n::record_t& record) {

    return { record.at("tag").asString(),
             record.at("content").asString(),
             split(record.at("skills").asString(), ", "),
             fromPGSQLFormat(record.at("last_updated_at").asString()) };
}

std::vector<std::string> Resume::projection() {
    return { "tag", "content", "skills", "last_updated_at" };
}

database_n::record_t Resume::toDatabaseFormat() const {
    return {{ "tag",             tag },
            { "content",         content },
            { "skills",          join(skills, ", ") },
            { "last_updated_at", toPGSQLFormat(Time::now()) }};
}

nlohmann::json Resume::toJSON() const {
    return nlohmann::json({{ "tag",             tag },
                           { "content",         content },
                           { "skills",          skills },
                           { "last_updated_at", Time::toSecondsSinceEpoch(last_updated_at) }});
}