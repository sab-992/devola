#include <service/rss.hpp>


RSS::RSS(const Private_s&, const json& configJSON) : m_configJSON(configJSON), Basic("RSS", configJSON["server"]["port"]) {
    setStartSequence([&](Basic*){
        assert(this->m_database && "[RSS]: No database given");
        // TODO: assert(this->m_cache && "[RSS]: No cache given");
        m_listingRepos = std::make_unique<ListingRepository>(tools());

    });
    setEndpoints();
}

RSS::~RSS() {}

std::unique_ptr<RSS> RSS::create(const json& configJSON) {
    return std::make_unique<RSS>(Private_s(), configJSON);
}

std::string RSS::pathPrefix() const {
    return "/rss";
}

void RSS::setCache(std::shared_ptr<Database_i> cache) {
    m_cache = cache;
}

void RSS::setDatabase(std::shared_ptr<Database_i> database) {
    m_database = database;
}

void RSS::setEndpoints() {
    ENDPOINT("GET", "/feed", &RSS::fetchFeeds);
}

rss::ServerTools RSS::tools() {
    return { m_cache, m_database, m_http };
}

asio::awaitable<http_n::Response> RSS::fetchFeeds(const Session& session, const http_n::Request& request) {
    using namespace http_n;
    using namespace database_n;

    // 1) TODO: Validate JWT and get client UUID.
    // 1.1) TODO: If JWT is not valid --> Return Unauthorized.
    // 1.2) TODO: If JWT valid but expired --> Return "Refresh token".

    // 2) TODO: Fetch subscribed urls from DB.
    // 2.1) TODO: If no urls --> Return empty.
    std::vector<std::string> subscribedURLs = { "https://weworkremotely.com/categories/remote-customer-support-jobs.rss",
                                                "https://remotive.com/remote-jobs/feed/software-development",
                                                "https://himalayas.app/jobs/rss",
                                                "https://jobicy.com/jobs/feed?industry=engineering" };

    auto response = Response();
    if (subscribedURLs.empty())
        co_return response.setStatus(network_n::Code::OK).build();

    json body = co_await m_listingRepos->fetchListings(subscribedURLs);

    co_return response.setStatus(network_n::Code::OK)
                      .setBody<nlohmann::json>(body).build();
}