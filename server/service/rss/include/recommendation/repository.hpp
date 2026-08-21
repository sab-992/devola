#pragma once

#include <core/process.hpp>
#include <core/logging.hpp>
#include <dataclass/resume.hpp>
#include <dataclass/recommendation.hpp>
#include <service/tools.hpp>


class RecommendationRepository {
    using record_t = database_n::record_t;
    using ServerTools = rss::ServerTools;
    using json = nlohmann::json;

    inline static const size_t N_SCORES_KEPT = 3;
    inline static const std::string NAMED_PIPE_NAME = "rss_matcher";

    template<class... Ts>
    struct overloads : Ts... { using Ts::operator()...; };

public:
    RecommendationRepository(const ServerTools& tools);
    ~RecommendationRepository() = default;

    friend std::unique_ptr<RecommendationRepository> std::make_unique<RecommendationRepository>();

    RecommendationTask createTask(std::string_view userUUID);
    json fetchTaskResult(std::string_view userUUID, std::string_view taskUUID);
    json fetchTasks(std::string_view userUUID);
    void runTask(std::string_view userUUID, std::string_view taskUUID, const std::vector<Resume>& resumes, const json& websiteListingsJSON, const std::vector<record_t>& subscriptions);

private:
    ServerTools m_tools;
    std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();
    std::shared_ptr<process_n::Registry> m_registry = process_n::Registry::instance();

    json buildJSONRecommendationBody(const std::vector<record_t>& records);
    std::shared_ptr<database_n::Database_i> database();
    bool sendResumes(processId_t processIdentifier, std::string_view taskUUID, const std::vector<Resume>& resumes) const;
    bool sendWebsiteListings(processId_t processIdentifier, std::string_view taskUUID, const json& websiteListingsJSON, const std::vector<record_t>& subscriptions) const;
    json sortScores(const json& rawScores) const;
    json topNScores(const json& scoresArray, size_t N=N_SCORES_KEPT) const;
};