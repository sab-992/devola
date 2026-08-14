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

asio::awaitable<http_n::Response> RSSService::addResumes(const Session& session, const http_n::Request& request) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const json& body = request.body<json>();
    if (not body.contains("resumes") or not body["resumes"].is_array())
        co_return Response().setStatus(Code::BAD_REQUEST).build();

    bool added = false;
    for (const auto& resumeJSON : body["resumes"]) {
        if (not resumeJSON.is_object())
            continue;

        m_resumeRepos->createResume(userInfo["uuid"].get<std::string>(), resumeJSON);
        added = true;
    }

    if (not added)
        co_return Response().setStatus(Code::BAD_REQUEST).build();

    co_return Response().setStatus(Code::CREATED).build();
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
    ENDPOINT("GET",  "/feed",            &RSSService::fetchFeeds);
    ENDPOINT("GET",  "/recommendation",  &RSSService::recommendation);
    ENDPOINT("GET",  "/recommendations", &RSSService::recommendations);

    ENDPOINT("POST", "/recommend",       &RSSService::recommend);

    ENDPOINT("PUT",  "/subscribe",       &RSSService::subscribe);
    ENDPOINT("PUT",  "/resumes",         &RSSService::addResumes);
}

rss::ServerTools RSSService::tools() {
    return { m_cache, m_database, m_http };
}

asio::awaitable<http_n::Response> RSSService::fetchFeeds(const Session& session, const http_n::Request& request) {
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

asio::awaitable<http_n::Response> RSSService::recommend(const Session& session, const http_n::Request& request) {
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

    const std::string& taskUUID = m_recommendationRepos->createTask(userUUID);
    asio::post(m_ioContext.get_executor(), std::bind_front(&RecommendationRepository::runTask, m_recommendationRepos, userUUID,
                                                                                                                      taskUUID,
                                                                                                                      resumes,
                                                                                                                      websiteListings,
                                                                                                                      subscriptions));
    co_return Response().setStatus(Code::OK)
                        .setBody<json>({ { "task_uuid", taskUUID }}).build();
}

asio::awaitable<http_n::Response> RSSService::recommendation(const Session& session, const http_n::Request& request) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const auto& body = m_recommendationRepos->fetchTaskResult(userInfo["uuid"].get<std::string>(), request.body<json>()["task_uuid"].get<std::string>());

    co_return Response().setBody<json>(body).setStatus(Code::OK).build();
}

asio::awaitable<http_n::Response> RSSService::recommendations(const Session& session, const http_n::Request& request) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const auto& body = m_recommendationRepos->fetchTaskResults(userInfo["uuid"].get<std::string>());


    co_return Response().setBody<json>(body).setStatus(Code::OK).build();
}

asio::awaitable<http_n::Response> RSSService::subscribe(const Session& session, const http_n::Request& request) {
    json userInfo;
    if (not validateUserIdentity(request, userInfo))
        co_return Response().setStatus(Code::UNAUTHORIZED).build();

    const nlohmann::json& body = request.body<nlohmann::json>();
    const std::vector<std::string>& urls = body["urls"].get<std::vector<std::string>>();

    m_subscriptionRepos->createSubscriptions(userInfo["uuid"].get<std::string>(), urls);
    co_await m_listingRepos->createWebsiteListings(urls);

    co_return Response().setStatus(Code::CREATED).build();
}

bool RSSService::validateUserIdentity(const http_n::Request& request, json& claims) const {
    try {
        claims = JWT::verify(JWT::getToken(request));
    } catch (const std::exception& e) {
        return false;
    }
    return true;
}