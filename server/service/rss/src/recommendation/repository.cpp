#include <recommendation/repository.hpp>


RecommendationRepository::RecommendationRepository(const ServerTools& tools) : m_tools(tools) {}

std::string RecommendationRepository::createTask(std::string_view userUUID) {
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

    return createTaskResult.records().value()[0].at("uuid").asString();
}

nlohmann::json RecommendationRepository::buildJSONRecommendationBody(const std::vector<record_t>& records) {
    json body = json::array();
    for (const auto& record : records) {
        json object;

        for (const auto& [col, val] : record) {
            std::visit(overloads{[&](bool arg) { object[col] = arg; },
                                 [&](int64_t arg) { object[col] = arg; },
                                 [&](double arg) { object[col] = arg; },
                                 [&](const std::string& arg) { object[col] = arg; },
                                 [&](auto&& arg) {}}, val.raw());
        }

        body.emplace_back(object);
    }

    return body;
}

std::shared_ptr<database_n::Database_i> RecommendationRepository::database() {
    return m_tools.database;
}

nlohmann::json RecommendationRepository::fetchTaskResult(std::string_view userUUID, std::string_view taskUUID) {
    using namespace database_n;

    const Result& result = database()->Read(Query().setTarget("get_task_results")
                                                   .setType(Query::Type_en::FUNCTION)
                                                   .setProjection({ "*" })
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFunctionData({ taskUUID }).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty())
        return {};

    return buildJSONRecommendationBody(result.records().value());
}

nlohmann::json RecommendationRepository::fetchTaskResults(std::string_view userUUID) {
    using namespace database_n;
    const Result& result = database()->Read(Query().setTarget("tasks")
                                                   .setType(Query::Type_en::TARGETED)
                                                   .setProjection({ "tr.*" })
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFilter({ { "tasks.user_uuid", { "=", Value(userUUID) }} })
                                                   .addJoin({ Query::JoinType_en::CROSS, "get_task_results(tasks.uuid)", std::nullopt, "tr"}).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty())
        return {};

    return buildJSONRecommendationBody(result.records().value());
}

void RecommendationRepository::runTask(std::string_view userUUID,
                                       std::string_view taskUUID,
                                       const std::vector<Resume>& resumes,
                                       const json& websiteListingsJSON,
                                       const std::vector<record_t>& subscriptions) {
    using namespace database_n;

    const Process& p = m_registry->start("python", { std::format("{}/extern/matcher.py", SERVICE_DIRECTORY) }, true);

    sendResumes(p.id(), resumes);
    sendWebsiteListings(p.id(), websiteListingsJSON, subscriptions);

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
                                        .setData({ { "status",          "completed" },
                                                   { "last_updated_at", toPGSQLFormat(Time::now()) }})
                                        .setFilter({ {"uuid", {"=", taskUUID } } }).build();

    const Result& updateTaskResult = database()->Update(updateTaskQuery);

    if (not updateTaskResult.isOK())
        throw Exception(updateTaskResult.error().value());
}

void RecommendationRepository::sendResumes(processId_t processIdentifier, const std::vector<Resume>& resumes) const {
    std::string resumeMessage;
    for (const auto& resume : resumes)
        resumeMessage += std::format("{}[SEP]{}[END]\n", resume.tag, resume.content);

    m_registry->send(processIdentifier, NAMED_PIPE_NAME, resumeMessage);

    std::string resumeResponse = m_registry->receive(processIdentifier, NAMED_PIPE_NAME);
    if (resumeResponse != "OK")
        m_light->log(log_n::Level_en::ERROR, "resumeResponse NOT OK");
}

void RecommendationRepository::sendWebsiteListings(processId_t processIdentifier, const json& websiteListingsJSON, const std::vector<record_t>& subscriptions) const {
    m_registry->send(processIdentifier, NAMED_PIPE_NAME, std::to_string(subscriptions.size()));
    for (const auto& websiteJSON : websiteListingsJSON) {
        if (websiteJSON.empty())
            continue;

        std::string websiteListings;
        for (const auto& listing : websiteJSON["listings"])
            websiteListings += std::format("{}[SEP]{}[END]\n", listing["id"].get<int64_t>(), listing["content"].get<std::string>());

        m_registry->send(processIdentifier, NAMED_PIPE_NAME, websiteListings);
        std::string websiteListingsResponse = m_registry->receive(processIdentifier, NAMED_PIPE_NAME);

        if (websiteListingsResponse != "OK")
            m_light->log(log_n::Level_en::ERROR, "websiteListingsResponse NOT OK");
    }
}

nlohmann::json RecommendationRepository::sortScores(const json& rawScores) const {
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

nlohmann::json RecommendationRepository::topNScores(const json& scoresArray, size_t N) const {
    json topN = json::array();
    for (size_t i = 0; i < std::min(size_t(N), scoresArray.size()); i++)
        topN.push_back(scoresArray[i]);
    return topN;
}