#pragma once

#include <dataclass/resume.hpp>
#include <service/tools.hpp>


class ResumeRepository {
    using record_t = database_n::record_t;
    using ServerTools = rss::ServerTools;
    using json = nlohmann::json;

public:
    ResumeRepository(const ServerTools& tools);
    ~ResumeRepository() = default;

    friend std::unique_ptr<ResumeRepository> std::make_unique<ResumeRepository>();

    void createResume(std::string_view userUUID, const json& resume);
    std::vector<Resume> fetchResumes(std::string_view userUUID);

private:
    ServerTools m_tools;

    std::shared_ptr<database_n::Database_i> database();
    database_n::record_t RecordFromJSON(std::string_view userUUID, const json& resumeInformation);
};