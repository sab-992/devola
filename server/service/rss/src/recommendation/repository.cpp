#include <recommendation/repository.hpp>


RecommendationRepository::RecommendationRepository(const ServerTools& tools) : m_tools(tools) {}

std::string RecommendationRepository::createTask(std::string_view userUUID, const std::vector<Resume>& resumes, const json& websiteListingsJSON, const std::vector<record_t>& subscriptions) {
    using namespace database_n;

    const auto createTaskQuery = Query().setTarget("tasks")
                                        .setProjection({ "uuid" })
                                        .setType(Query::Type_en::TARGETED)
                                        .setCardinality(Query::Cardinality_en::SINGLE)
                                        .setData({ { "user_uuid", userUUID },
                                                   { "status", "running" }}).build();

    const Result& createTaskResult = database()->Create(createTaskQuery);

    if (not createTaskResult.isOK())
        throw Exception(createTaskResult.error().value());
    else if (createTaskResult.isEmpty())
        throw Exception("Task UUID is needed to start a recommendation task");

    const std::string taskUUID = createTaskResult.records().value()[0].at("task_uuid").asString();
    // =============================  In asio::post  ================================
    const Process& p = m_registry->start("python", { std::format("{}/extern/matcher.py", SERVICE_DIRECTORY) }, true);

    // build resume
    std::string resumeMessage;
    for (const auto& resume : resumes)
        resumeMessage += std::format("{}[SEP]{}[END]\n", resume.tag, resume.content);

    const std::string NAMED_PIPE_NAME = "rss_matcher";
    m_registry->send(p.id(), NAMED_PIPE_NAME, resumeMessage);
    // send resumes
    std::string resumeResponse = m_registry->receive(p.id(), NAMED_PIPE_NAME);
    if (resumeResponse != "OK")
        m_light->log(log_n::Level_en::ERROR, "resumeResponse NOT OK");

    // send how many websites
    m_registry->send(p.id(), NAMED_PIPE_NAME, std::to_string(subscriptions.size()));
    for (const auto& websiteJSON : websiteListingsJSON) {
        if (websiteJSON.empty())
            continue;

        // build website listings
        std::string websiteListings;
        for (const auto& listing : websiteJSON["listings"])
            websiteListings += std::format("{}[SEP]{}[END]\n", listing["id"].get<int64_t>(), listing["content"].get<std::string>());

        // send website listings
        m_registry->send(p.id(), NAMED_PIPE_NAME, websiteListings);
        std::string websiteListingsResponse = m_registry->receive(p.id(), NAMED_PIPE_NAME);
        if (websiteListingsResponse != "OK")
            m_light->log(log_n::Level_en::ERROR, "websiteListingsResponse NOT OK");
    }

    const auto rawScores = nlohmann::json::parse(m_registry->receive(p.id(), NAMED_PIPE_NAME));

    const auto& scores = sortScores(rawScores);
    const auto createTaskResultQuery = Query().setTarget("task_results")
                                              .setType(Query::Type_en::TARGETED)
                                              .setCardinality(Query::Cardinality_en::NONE)
                                              .setData({ { "user_uuid", userUUID },
                                                         { "task_uuid", taskUUID },
                                                         { "result", scores.dump() }}).build();

    const Result& createTaskResultResult = database()->Create(createTaskResultQuery);

    if (not createTaskResultResult.isOK())
        throw Exception(createTaskResultResult.error().value());

    const auto updateTaskQuery = Query().setTarget("tasks")
                                        .setType(Query::Type_en::TARGETED)
                                        .setCardinality(Query::Cardinality_en::NONE)
                                        .setData({ { "status", "completed" }}).build();

    const Result& updateTaskResult = database()->Update(updateTaskQuery);

    if (not updateTaskResult.isOK())
        throw Exception(updateTaskResult.error().value());

    // TODO: remove
    file_n::Service::instance()->open("/home/rysa/Desktop/random/test.json", file_n::flags_n::OpenMode_en::WRITE, file_n::flags_n::OpenMode_en::TRUNCATE).write(rawScores.dump());
    file_n::Service::instance()->open("/home/rysa/Desktop/random/testsorted.json", file_n::flags_n::OpenMode_en::WRITE, file_n::flags_n::OpenMode_en::TRUNCATE).write(scores.dump());

    // ==============================================================================

    return taskUUID;
}

std::string RecommendationRepository::fetchTask(std::string_view userUUID, std::string_view taskUUID) { return ""; }
std::string RecommendationRepository::fetchTasks(std::string_view userUUID) { return ""; }

std::shared_ptr<database_n::Database_i> RecommendationRepository::database() {
    return m_tools.database;
}

nlohmann::json RecommendationRepository::sortScores(const json& rawScores) {
    json result = json::array();
    for (const auto& item : rawScores) {
        json websiteListingScores;
        websiteListingScores["listing_ids"] = item["listing_ids"];

        json newScores = json::array();

        for (auto sorted : item["scores"]) {
            std::sort(sorted.begin(), sorted.end(), [](const json& a, const json& b) { return a["score"].get<double>() > b["score"].get<double>(); });
            newScores.push_back(topNScores(sorted));
        }

        websiteListingScores["scores"] = newScores;
        result.push_back(websiteListingScores);
    }
    return result;
}

nlohmann::json RecommendationRepository::topNScores(const json& scoresArray, size_t N) {
    json topN = json::array();
    for (size_t i = 0; i < std::min(size_t(N), scoresArray.size()); i++)
        topN.push_back(scoresArray[i]);
    return topN;
}