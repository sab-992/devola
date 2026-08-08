#include <listing/repository.hpp>


ListingRepository::ListingRepository(const ServerTools& tools) : m_tools(tools) {}

std::shared_ptr<database_n::Database_i> ListingRepository::cache() {
    return m_tools.cache;
}

std::shared_ptr<database_n::Database_i> ListingRepository::database() {
    return m_tools.database;
}

const std::unique_ptr<http_n::Http>& ListingRepository::http() {
    return m_tools.http;
}

asio::awaitable<nlohmann::json> ListingRepository::fetchListings(const std::vector<std::string>& subscribedURLs) {
    using namespace http_n;
    using namespace database_n;

    json body = json::array();

    auto tx = Transaction(pgsql_n::Transaction());
    for (const auto& url : subscribedURLs) {
        auto [host, endpoint] = parseURL(url);

        json websiteListings;

        // 3.1) TODO: check in cache:
        //     3.1.1) TODO: if in cache and not expired, add it.

        const auto& [last_updated, expire_at, listings] = fetchWebsiteListings(host, endpoint, tx);
        if (last_updated.empty() or listings.empty())
            websiteListings= co_await saveWebsiteListingsToDatabase(host, endpoint, tx);
        else if (isExpired(fromPGSQLFormat(expire_at))) {
            // TODO: remove when updateWebsiteListingsDatabase is completed
            m_light->log(log_n::Level_en::WARNING, "Website", host, "listings are expired. Expiry date:", expire_at);
            websiteListings = co_await updateWebsiteListingsDatabase(host, endpoint, tx);
        }
        else
            websiteListings = listingsToJSON(host, last_updated, expire_at, listings);

        // 3.4) TODO: save in cache
        // saveWebsiteListingsToCache(host, endpoint, websiteListings);

        body.emplace_back(websiteListings);
    }
    tx.get<pgsql_n::Transaction>().commit();

    co_return body;
}

asio::awaitable<http_n::Response> ListingRepository::fetchFromURL(std::string_view host, std::string_view endpoint) {
    constexpr size_t MAX_WEBSITE_LEN  = 253;
    if (host.size() > MAX_WEBSITE_LEN)
        throw InvalidArgument(std::format("exceeds maximum size of {} characters", MAX_WEBSITE_LEN), "Hostname");

    constexpr size_t MAX_ENDPOINT_LEN = 255;
    if (endpoint.size() > MAX_ENDPOINT_LEN)
        throw InvalidArgument(std::format("exceeds maximum size of {} characters", MAX_ENDPOINT_LEN), "Endpoint");

    auto request = http_n::Request().setMethod("GET")
                                    .setURL(host)
                                    .setAPIEndpoint(endpoint).build();

    co_return co_await http()->async_receive(co_await http()->async_send(request));
}

std::tuple<std::string, std::string, std::vector<Listing>> ListingRepository::fetchWebsiteListings(std::string_view host, std::string_view endpoint, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    auto websiteExistsQuery = Query().setTarget("websites")
                                     .setType(Query::Type_en::TARGETED)
                                     .setProjection({ "last_updated", "expire_at", "listings.*" })
                                     .setCardinality(Query::Cardinality_en::MULTIPLE)
                                     .setFilter({{ "host",     { "=", Value(host) }},
                                                 { "endpoint", { "=", Value(endpoint) }}})
                                     .addJoin({ Query::JoinType_en::INNER,
                                                "listings",
                                                std::nullopt,
                                                std::nullopt,
                                                {{"websites.host", "=", "listings.website_host"},
                                                 {"websites.endpoint", "=", "listings.website_endpoint"}}}).build();

    Result websiteExistsResult = database()->Read(websiteExistsQuery, &tx);

    if (not websiteExistsResult.isOK())
        throw Exception(websiteExistsResult.error().value());

    if (websiteExistsResult.isEmpty())
        return { "", "",  {} };

    const auto& recordsOpt = websiteExistsResult.records();
    std::vector<Listing> listings;
    for (const auto& record : recordsOpt.value())
        listings.emplace_back(Listing::fromDatabaseFormat(record));

    return { recordsOpt.value().at(0).at("last_updated").asString(),
             recordsOpt.value().at(0).at("expire_at").asString(),
             listings };
}

bool ListingRepository::isExpired(const std::chrono::time_point<std::chrono::system_clock>& expiryTimestamp) const {
    return expiryTimestamp < Time::now();
}

nlohmann::json ListingRepository::listingsToJSON(std::string_view host,
                                                 std::string_view lastUpdated,
                                                 std::string_view expireAt,
                                                 const std::vector<Listing>& listings) const {
    json websiteListings;
    websiteListings["website_name"] = host;
    websiteListings["expire_at"] = Time::convertToSecondsSinceEpoch(Time::timepoint("%Y-%m-%d %H:%M:%S", expireAt));
    websiteListings["last_updated"] = Time::convertToSecondsSinceEpoch(Time::timepoint("%Y-%m-%d %H:%M:%S", lastUpdated));

    json listingsJSON = json::array();
    for (const auto& listing : listings)
        listingsJSON.push_back(listing.ToDatabaseFormat());

    websiteListings["listings"] = listingsJSON;
    return websiteListings;
}

asio::awaitable<nlohmann::json> ListingRepository::saveWebsiteListingsToDatabase(std::string_view host, std::string_view endpoint, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    Response feed = co_await fetchFromURL(host, endpoint);
    const auto& [ttl, listings] = parser_n::Listings::parse(host, endpoint, feed.body<xml_n::Document>());
    const auto& expireAt = toPGSQLFormat(Time::now() + std::chrono::minutes(ttl));

    auto websiteQuery = Query().setTarget("websites")
                               .setType(Query::Type_en::TARGETED)
                               .setProjection({ "last_updated", "expire_at" })
                               .setCardinality(Query::Cardinality_en::SINGLE)
                               .setData({ { "host",      Value(host) },
                                          { "endpoint",  Value(endpoint) },
                                          { "expire_at", Value(expireAt) } }).build();

    Result result = database()->Create(websiteQuery, &tx);

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (not result.records()->at(0).contains("last_updated"))
        throw Exception("Cannot parse listings without last updated timestamp");

    nlohmann::json websiteListings = listingsToJSON(host,
                                                    result.records()->at(0).at("last_updated").asString(),
                                                    result.records()->at(0).at("expire_at").asString(),
                                                    listings);

    auto listingsQuery = Query().setTarget("insert_listings_batch")
                                .setType(Query::Type_en::PROCEDURE)
                                .setCardinality(Query::Cardinality_en::NONE)
                                .setData({ { "payload", Value(websiteListings["listings"].dump()) } }).build();

    Result listingsResult = database()->Other(listingsQuery, &tx);
    if (not listingsResult.isOK())
        throw Exception(listingsResult.error().value());

    co_return websiteListings;
}

asio::awaitable<nlohmann::json> ListingRepository::updateWebsiteListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx) {
    // TODO
    co_return listingsToJSON(host, "", "", {});
}