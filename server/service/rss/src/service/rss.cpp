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
    ENDPOINT("POST", "/recommend", &RSS::recommend);
    ENDPOINT("GET", "/recommendations", &RSS::recommendations);
}

rss::ServerTools RSS::tools() {
    return { m_cache, m_database, m_http };
}

asio::awaitable<http_n::Response> RSS::fetchFeeds(const Session& session, const http_n::Request& request) {
    using namespace http_n;
    using namespace database_n;

    // 1) Validate JWT and get user id.
    // 1.1) If JWT is not valid --> Return Unauthorized.
    // 1.2) If JWT valid but expired --> Return "Refresh token".

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

asio::awaitable<http_n::Response> RSS::recommend(const Session& session, const http_n::Request& request) {
    // TODO
    // 1) Validate JWT and get user id.
    // 1.1) If JWT is not valid --> Return Unauthorized.
    // 1.2) If JWT valid but expired --> Return "Refresh token".

    // fetch resumes of the user
    // fetch subscribed urls of the user
    // fetch listings using urls

    // Maybe start background task on background thread:
        // 1) RSS -> Send resumes to MATCHER process
        //     1.1) Each resume will have this structure: "<tag>:<resume>[END];". (tag refers to words given by the user to identify the resume)

        // 2) MATCHER -> Send to RSS process "OK"

        // 3) RSS -> Sends listings to MATCHER process
        //     3.1) Each listing will have this structure: "<listing id>:<listing>[END];"

        // 4) MATCHER -> for each listing:
        //     4.1) Clean listing from HTML junk.
        //     4.2) Chunk listing
        //     4.3) Compute embedding
        //     4.4) for each resume:
        //         4.4.1) Chunk resume
        //         4.4.2) Compute embedding
        //         4.4.3) Compute cosine similarity of both
        //         4.4.4) Store result in local array
        //     4.5) Get top 3 resumes + scores for the listing and Save them in response

        // 5) MATCHER -> Send To RSS response
        //     5.1) Response will look like this:
        //         { "listing_ids": [...],
        //         "scores": [[{"tag": "...", "score": ... }], ...]}
        //         5.1.1) Listing ids and scores are separate because when the user will get the result, RSS will have to grab the link of the listing corresponding to the score. an easy way to do that would be to fetch all ids needed and then match them to the scores and send the information.

        // 6) RSS -> Saves response in response database and maybe add host FK to send to the client too.

    co_return  http_n::Response().setStatus(network_n::Code::OK).build();
}

asio::awaitable<http_n::Response> RSS::recommendations(const Session& session, const http_n::Request& request) {
    // TODO
    // 1) Validate JWT and get user id.
    // 1.1) If JWT is not valid --> Return Unauthorized.
    // 1.2) If JWT valid but expired --> Return "Refresh token".

    // 1 task contains 1 or more results. As results is the recommendation for each listing of ONE website.
    // Therefore it needs:
        // "task" table -> PK on ("user_id" and "uuid") = FK on "user_id", INDEX on "uuid", "started_at", "status", "last_update"
        // "result" table -> PK on ("user_id" and "id") = "id" : generated, FK on "user_id", FK and INDEX on "task_uuid", FK on "host", "result".

    // 2) Fetch user's lists of tasks (containing results)
    co_return  http_n::Response().setStatus(network_n::Code::OK).build();
}