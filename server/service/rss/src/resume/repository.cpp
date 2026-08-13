#include <resume/repository.hpp>


ResumeRepository::ResumeRepository(const ServerTools& tools) : m_tools(tools) {}

void ResumeRepository::createResume(std::string_view userUUID, const json& resume) {
    using namespace database_n;

    const auto createResumeQuery = Query().setTarget("resumes")
                                          .setType(Query::Type_en::TARGETED)
                                          .setCardinality(Query::Cardinality_en::NONE)
                                          .setData(RecordFromJSON(userUUID, resume)).build();

    const Result& result = database()->Create(createResumeQuery);

    if (not result.isOK())
        throw Exception(result.error().value());
}

std::shared_ptr<database_n::Database_i> ResumeRepository::database() {
    return m_tools.database;
}

std::vector<Resume> ResumeRepository::fetchResumes(std::string_view userUUID) {
    using namespace database_n;

    const auto fetchUserQuery = Query().setTarget("resumes")
                                       .setProjection(Resume::projection())
                                       .setType(Query::Type_en::TARGETED)
                                       .setCardinality(Query::Cardinality_en::MULTIPLE)
                                       .setFilter({ { "user_uuid", { "=", userUUID } } }).build();

    const Result& result = database()->Read(fetchUserQuery);

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (not result.records().has_value())
        return {};

    std::vector<Resume> resumes;
    const auto& recordsOPT = result.records();
    for (const auto& resumeRecord : recordsOPT.value())
        resumes.emplace_back(Resume::fromDatabaseFormat(resumeRecord));

    return resumes;
}

database_n::record_t ResumeRepository::RecordFromJSON(std::string_view userUUID, const json& resumeInformation) {
    record_t resumeRecord;
    resumeRecord["user_uuid"] = userUUID;
    for (auto& [key, value] : resumeInformation.items()) {
        if (value.is_null() or not value.is_string())
            continue;

        resumeRecord[key] = value.get<std::string>();
    }
    return resumeRecord;
}