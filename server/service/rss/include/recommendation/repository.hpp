#pragma once

#include <core/process.hpp>
#include <core/logging.hpp>
#include <dataclass/resume.hpp>
#include <service/tools.hpp>


class RecommendationRepository {
    using record_t = database_n::record_t;
    using ServerTools = rss::ServerTools;
    using json = nlohmann::json;

    static const size_t N_SCORES_KEPT = 3;

public:
    RecommendationRepository(const ServerTools& tools);
    ~RecommendationRepository() = default;

    friend std::unique_ptr<RecommendationRepository> std::make_unique<RecommendationRepository>();

    std::string createTask(std::string_view userUUID, const std::vector<Resume>& resumes, const json& websiteListingsJSON, const std::vector<record_t>& subscriptions);
    std::string fetchTasks(std::string_view userUUID);
    std::string fetchTask(std::string_view userUUID, std::string_view taskUUID);

private:
    ServerTools m_tools;
    std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();
    std::shared_ptr<process_n::Registry> m_registry = process_n::Registry::instance();

    std::shared_ptr<database_n::Database_i> database();

    json sortScores(const json& rawScores);
    json topNScores(const json& scoresArray, size_t N=N_SCORES_KEPT);
};