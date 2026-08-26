#include <service/rss.hpp>


RSSService::RSSService(const Private_s&, const json& configJSON) : m_configJSON(configJSON), Basic("RSSService", configJSON["server"]["port"]) {
    setStartSequence([&](Basic*){
        assert(this->m_database && "[RSSService]: No database given");
        // TODO: assert(this->m_cache && "[RSSService]: No cache given");

        m_listingRepos = std::make_unique<ListingRepository>(tools());
        m_recommendationRepos = std::make_shared<RecommendationRepository>(tools());
        m_resumeRepos = std::make_unique<ResumeRepository>(tools());
        m_subscriptionRepos = std::make_unique<SubscriptionRepository>(tools());
    });
    setEndpoints();
}

RSSService::~RSSService() {}

asio::awaitable<http_n::Response> RSSService::addResume(const Session& session, const http_n::Request& request, const pathParams_t&) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const json& requestBody = request.body<json>();
    if (not requestBody.is_object())
        co_return Response().setStatus(Code::BAD_REQUEST).build();

    try {
        const auto& addedResume = m_resumeRepos->createResume(userInfo["uuid"].get<std::string>(), requestBody);
        co_return Response().setBody(addedResume.toJSON())
                            .setStatus(Code::CREATED).build();
    } catch (std::exception e) {
        m_light->log(log_n::Level_en::ERROR, "while creating resume:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

asio::awaitable<http_n::Response> RSSService::deleteResume(const Session& session, const http_n::Request& request, const pathParams_t& params) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    if (not params.contains("tag"))
        co_return Response().setStatus(Code::NOT_ALLOWED).build();

    try {
        m_resumeRepos->deleteResume(userInfo["uuid"].get<std::string>(), params.at("tag"));
        co_return Response().setStatus(Code::NO_CONTENT).build();
    } catch (std::exception e) {
        m_light->log(log_n::Level_en::ERROR, "while deleting resume:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

std::unique_ptr<RSSService> RSSService::create(const json& configJSON) {
    return std::make_unique<RSSService>(Private_s(), configJSON);
}

std::string RSSService::pathPrefix() const {
    return "/rss";
}

void RSSService::setCache(std::shared_ptr<Database_i> cache) {
    m_cache = cache;
}

void RSSService::setDatabase(std::shared_ptr<Database_i> database) {
    m_database = database;
}

void RSSService::setEndpoints() {
    ENDPOINT("DELETE", "/resumes/{tag}",               &RSSService::deleteResume);

    ENDPOINT("GET",    "/feed",                        &RSSService::feeds);
    ENDPOINT("GET",    "/recommendations",             &RSSService::recommendations);
    ENDPOINT("GET",    "/recommendations/{task_uuid}", &RSSService::recommendation);
    ENDPOINT("GET",    "/resumes",                     &RSSService::resumes);
    ENDPOINT("GET",    "/subscriptions",               &RSSService::subscriptions);

    ENDPOINT("PATCH",   "/resumes/{tag}",              &RSSService::updateResume);

    ENDPOINT("POST",   "/recommendations",             &RSSService::recommend);
    ENDPOINT("POST",   "/resumes",                     &RSSService::addResume);

    ENDPOINT("PUT",    "/subscriptions",               &RSSService::subscribe);
}

rss::ServerTools RSSService::tools() {
    return { m_cache, m_database, m_http };
}

asio::awaitable<http_n::Response> RSSService::feeds(const Session& session, const http_n::Request& request, const pathParams_t&) {
    using namespace database_n;

    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const auto& subscriptions = m_subscriptionRepos->fetchSubscriptions(userInfo["uuid"].get<std::string>());

    auto response = Response();
    if (subscriptions.empty())
        co_return response.setStatus(Code::OK).build();

    json body = co_await m_listingRepos->fetchListings(subscriptions);

    co_return response.setStatus(Code::OK)
                      .setBody<nlohmann::json>(body).build();
}

asio::awaitable<http_n::Response> RSSService::recommend(const Session& session, const http_n::Request& request, const pathParams_t&) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const std::string& userUUID = userInfo["uuid"].get<std::string>();
    const std::vector<Resume>& resumes = m_resumeRepos->fetchResumes(userUUID);
    const auto& subscriptions = m_subscriptionRepos->fetchSubscriptions(userUUID);

    if (subscriptions.empty())
        co_return Response().setStatus(Code::BAD_REQUEST).build();
    else if (resumes.empty())
        co_return Response().setStatus(Code::BAD_REQUEST).build();

    json websiteListings = co_await m_listingRepos->fetchListings(subscriptions);

    const RecommendationTask& task = m_recommendationRepos->createTask(userUUID);
    asio::post(m_ioContext.get_executor(), std::bind_front(&RecommendationRepository::runTask, m_recommendationRepos, userUUID,
                                                                                                                      task.uuid,
                                                                                                                      resumes,
                                                                                                                      websiteListings,
                                                                                                                      subscriptions));
    co_return Response().setStatus(Code::OK)
                        .setBody<json>(task.toJSON()).build();
}

asio::awaitable<http_n::Response> RSSService::recommendation(const Session& session, const http_n::Request& request, const pathParams_t& params) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    if (not params.contains("task_uuid"))
        co_return Response().setStatus(Code::NOT_ALLOWED).build();

    const auto& body = m_recommendationRepos->fetchTaskResult(userInfo["uuid"].get<std::string>(), params.at("task_uuid"));

    co_return Response().setBody<json>(body)
                        .setStatus(Code::OK).build();
}

asio::awaitable<http_n::Response> RSSService::recommendations(const Session& session, const http_n::Request& request, const pathParams_t&) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const auto& body = m_recommendationRepos->fetchTasks(userInfo["uuid"].get<std::string>());


    co_return Response().setBody<json>(body)
                        .setStatus(Code::OK).build();
}

asio::awaitable<http_n::Response> RSSService::resumes(const Session& session, const http_n::Request& request, const pathParams_t&) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    try {
        const auto& resumes = m_resumeRepos->fetchResumes(userInfo["uuid"].get<std::string>());

        json body = json::array();
        for (const auto& resume : resumes)
            body.emplace_back(resume.toJSON());

        co_return Response().setBody(body)
                            .setStatus(Code::OK).build();
    } catch (std::exception e) {
        m_light->log(log_n::Level_en::ERROR, "while fetching resumes:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

asio::awaitable<http_n::Response> RSSService::subscribe(const Session& session, const http_n::Request& request, const pathParams_t&) {
    using namespace database_n;

    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    json subscriptions = request.body<nlohmann::json>();
    Transaction tx{pgsql_n::Transaction()};
    try {
        m_listingRepos->createWebsites(subscriptions, tx);
        m_subscriptionRepos->saveSubscriptions(userInfo["uuid"].get<std::string>(), subscriptions, tx);
        tx.get<pgsql_n::Transaction>().commit();
    } catch (std::exception e) {
        m_light->log(log_n::Level_en::ERROR, m_extraLogInformation, "Error during subscription to websites");
        tx.get<pgsql_n::Transaction>().abort();
        co_return Response().setStatus(Code::BAD_REQUEST).setBody<std::string>(e.what()).build();
    }

    co_return Response().setStatus(Code::CREATED).build();
}

asio::awaitable<http_n::Response> RSSService::subscriptions(const Session& session, const http_n::Request& request, const pathParams_t&) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const auto& subscriptions = m_subscriptionRepos->fetchSubscriptions(userInfo["uuid"].get<std::string>());
    json body = json::array();
    for (const auto& subscription : subscriptions) {
        json website;
        website["host"] = subscription.at("website_host").asString();
        website["endpoint"] = subscription.at("website_endpoint").asString();
        website["created_at"] = Time::toSecondsSinceEpoch(fromPGSQLFormat(subscription.at("created_at").asString()));

        body.emplace_back(website);
    }

    co_return Response().setBody<json>(body)
                        .setStatus(Code::OK).build();
}

asio::awaitable<http_n::Response> RSSService::updateResume(const Session& session, const http_n::Request& request, const pathParams_t& params) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    if (not params.contains("tag"))
        co_return Response().setStatus(Code::NOT_ALLOWED).build();

    const json& requestBody = request.body<json>();
    if (requestBody.empty() or not requestBody.is_array())
        co_return Response().setStatus(Code::BAD_REQUEST).build();

    try {
        Resume updatedResume = m_resumeRepos->updateResumeSkills(userInfo["uuid"].get<std::string>(), params.at("tag"), requestBody.get<std::vector<std::string>>());
        co_return Response().setBody<json>(updatedResume.toJSON())
                            .setStatus(Code::OK).build();
    } catch (std::exception e) {
        m_light->log(log_n::Level_en::ERROR, "while updating resume:", e.what());
        co_return Response().setStatus(Code::SERVER_ERROR).build();
    }
}

bool RSSService::validateUserIdentity(const http_n::Request& request, json& claims) const {
    try {
        claims = JWT::verify(JWT::getToken(request));
    } catch (const std::exception& e) {
        return false;
    }
    return true;
}