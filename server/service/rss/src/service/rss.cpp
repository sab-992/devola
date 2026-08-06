#include <service/rss.hpp>


RSS::RSS(const Private_s&, const json& configJSON) : m_configJSON(configJSON), Basic("RSS", configJSON["server"]["port"]) {
    setStartSequence([&](Basic*){
        assert(this->m_database && "[RSS]: No database given");
        // TODO: assert(this->m_cache && "[RSS]: No cache given");
    });
    setEndpoints();
}

RSS::~RSS() {}

std::unique_ptr<RSS> RSS::create(const json& configJSON) {
    return std::make_unique<RSS>(Private_s(), configJSON);
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

    nlohmann::json body = nlohmann::json::array();

    auto tx = Transaction(pgsql_n::Transaction());
    for (const auto& url : subscribedURLs) {
        auto [host, endpoint] = parseURL(url);

        // 3) TODO: Check Redis cache if has listings
        // 3.1) TODO: If in Redis and not expired --> Return feed.
        // 3.2) TODO: If in Redis cache but expired --> Refresh Redis cache and DB

        // 4) TODO: If not in Redis cache, check DB.
        // 4.1) TODO: If in DB and not expired --> Return feed.
        body.emplace_back(co_await updateListingsDatabase(host, endpoint, tx));
        // 5) TODO: refresh listings cache with the website JSON object
    }
    tx.get<pgsql_n::Transaction>().commit();

    co_return response.setStatus(network_n::Code::OK)
                      .setBody<nlohmann::json>(body).build();
}

asio::awaitable<http_n::Response> RSS::fetchFromURL(std::string_view host, std::string_view endpoint) {
    constexpr size_t MAX_WEBSITE_LEN  = 253;
    if (host.size() > MAX_WEBSITE_LEN)
        throw InvalidArgument(std::format("exceeds maximum size of {} characters", MAX_WEBSITE_LEN), "Hostname");

    constexpr size_t MAX_ENDPOINT_LEN = 255;
    if (endpoint.size() > MAX_ENDPOINT_LEN)
        throw InvalidArgument(std::format("exceeds maximum size of {} characters", MAX_ENDPOINT_LEN), "Endpoint");

    auto request = http_n::Request().setMethod("GET")
                                    .setURL(host)
                                    .setAPIEndpoint(endpoint).build();

    co_return co_await m_http->async_receive(co_await m_http->async_send(request));
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

asio::awaitable<nlohmann::json> RSS::updateListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    Response feed = co_await fetchFromURL(host, endpoint);

    // 1) TODO: Check if website exists
    // 1.1) TODO:  If exists: read the id, update last_updated and return it

    auto websiteQuery = Query().setTarget("websites")
                               .setType(Query::Type_en::TARGETED)
                               .setProjection({ "id", "last_updated" })
                               .setCardinality(Query::Cardinality_en::SINGLE)
                               .setData({ { "host",     Value(host) },
                                          { "endpoint", Value(endpoint) } }).build();

    Result result = m_database->Create(websiteQuery, &tx);

    if (not result.isOK())
        throw Exception(result.error().value());

    if (not result.records()->at(0).contains("id"))
        throw Exception("Cannot parse listings without website ID");
    else if (not result.records()->at(0).contains("last_updated"))
        throw Exception("Cannot parse listings without last updated timestamp");

    const std::vector<Listing>& listings = parser_n::Listings::parse(result.records()->at(0).at("id").asInt64(), feed.body<xml_n::Document>());

    nlohmann::json jsonListings = nlohmann::json::array();
    for (const auto& listing : listings)
        jsonListings.push_back(listing.databaseFormat());

    auto listingsQuery = Query().setTarget("insert_listings_batch")
                                .setType(Query::Type_en::PROCEDURE)
                                .setCardinality(Query::Cardinality_en::NONE)
                                .setData({ { "payload", Value(jsonListings.dump()) } }).build();

    Result listingsResult = m_database->Other(listingsQuery, &tx);
    if (not listingsResult.isOK())
        throw Exception(listingsResult.error().value());

    nlohmann::json websiteFeed;
    websiteFeed["website_name"] = host;
    websiteFeed["last_updated"] = Time::convertToSecondsSinceEpoch(Time::timepoint("%Y-%m-%d %H:%M:%S", result.records()->at(0).at("last_updated").asString()));
    websiteFeed["listings"] = jsonListings;

    co_return websiteFeed;
}