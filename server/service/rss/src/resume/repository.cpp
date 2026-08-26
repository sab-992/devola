#include <resume/repository.hpp>


ResumeRepository::ResumeRepository(const ServerTools& tools) : m_tools(tools) {}

Resume ResumeRepository::createResume(std::string_view userUUID, const json& resume) {
    using namespace database_n;

    const Result& result = database()->Create(Query().setTarget("resumes")
                                                     .setProjection(Resume::projection())
                                                     .setType(Query::Type_en::TARGETED)
                                                     .setCardinality(Query::Cardinality_en::SINGLE)
                                                     .setData(RecordFromJSON(userUUID, resume)).build());

    if (not result.isOK())
        throw Exception(result.error().value());

    return Resume::fromDatabaseFormat(result.records().value()[0]);
}

std::shared_ptr<database_n::Database_i> ResumeRepository::database() {
    return m_tools.database;
}

void ResumeRepository::deleteResume(std::string_view userUUID, std::string_view tag) {
    using namespace database_n;

    const Result& result = database()->Delete(Query().setTarget("resumes")
                                                     .setType(Query::Type_en::TARGETED)
                                                     .setCardinality(Query::Cardinality_en::NONE)
                                                     .setFilter({{ "user_uuid", { "=", userUUID } },
                                                                 { "tag",       { "=", tag }}}).build());

    if (not result.isOK())
        throw Exception(result.error().value());
}

std::vector<Resume> ResumeRepository::fetchResumes(std::string_view userUUID) {
    using namespace database_n;

    const Result& result = database()->Read(Query().setTarget("resumes")
                                                   .setProjection(Resume::projection())
                                                   .setType(Query::Type_en::TARGETED)
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFilter({{ "user_uuid", { "=", userUUID } }}).build());

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

Resume ResumeRepository::updateResumeSkills(std::string_view userUUID, std::string_view resumeTag, const std::vector<std::string>& updatedSkills) {
    using namespace database_n;

    const Result& result = database()->Update(Query().setTarget("resumes")
                                                     .setType(Query::Type_en::TARGETED)
                                                     .setProjection(Resume::projection())
                                                     .setCardinality(Query::Cardinality_en::SINGLE)
                                                     .setData({{ "skills", join(updatedSkills, ", ") }})
                                                     .setFilter({{ "user_uuid", { "=", userUUID } },
                                                                 { "tag",       { "=", resumeTag }}}).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty())
        throw Exception(std::format("Empty results - {}", FUNCTION_SIGNATURE));

    return Resume::fromDatabaseFormat(result.records().value()[0]);
}

database_n::record_t ResumeRepository::RecordFromJSON(std::string_view userUUID, const json& resumeInformation) {
    record_t resumeRecord = Resume::fromJSON(resumeInformation).toDatabaseFormat();
    resumeRecord["user_uuid"] = userUUID;
    return resumeRecord;
}