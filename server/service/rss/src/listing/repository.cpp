#include <listing/repository.hpp>


ListingRepository::ListingRepository(const ServerTools& tools) : m_tools(tools) {}

std::shared_ptr<database_n::Database_i> ListingRepository::cache() {
    return m_tools.cache;
}

asio::awaitable<void> ListingRepository::createListings(std::string_view host, std::string_view endpoint, std::string_view last_updated_at, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    const Response& feed = co_await fetchFromURL(host, endpoint);
    const nlohmann::json& websiteListings = listingsToJSON(host, last_updated_at, parser_n::Listings::parse(host, endpoint, feed.body<xml_n::Document>()));
    const auto listingsQuery = Query().setTarget("insert_listings_batch")
                                      .setType(Query::Type_en::PROCEDURE)
                                      .setCardinality(Query::Cardinality_en::NONE)
                                      .setData({ { "payload", Value(websiteListings["listings"].dump()) } }).build();
    const Result& result = database()->Other(listingsQuery, &tx);
}

std::string ListingRepository::createWebsite(std::string_view host, std::string_view endpoint, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    const auto websiteQuery = Query().setTarget("websites")
                                     .setType(Query::Type_en::TARGETED)
                                     .setProjection({ "last_updated_at" })
                                     .setCardinality(Query::Cardinality_en::SINGLE)
                                     .setData({ { "host",      Value(host) },
                                                   { "endpoint",  Value(endpoint) } }).build();

    const Result& result = database()->Create(websiteQuery, &tx);

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (not result.records()->at(0).contains("last_updated_at"))
        throw Exception("Cannot fetch listings without last updated timestamp");

    return result.records()->at(0).at("last_updated_at").asString();
}

asio::awaitable<void> ListingRepository::createWebsiteListings(const std::vector<std::string>& urls) {
    using namespace http_n;
    using namespace database_n;

    auto tx = Transaction(pgsql_n::Transaction());
    for (const auto& url : urls) {
        const auto& [host, endpoint] = parseURL(url);
        const auto& last_updated_at = createWebsite(host, endpoint, tx);
        co_await createListings(host, endpoint, last_updated_at, tx);
    }
    tx.get<pgsql_n::Transaction>().commit();
}

std::shared_ptr<database_n::Database_i> ListingRepository::database() {
    return m_tools.database;
}

const std::unique_ptr<http_n::Http>& ListingRepository::http() {
    return m_tools.http;
}

asio::awaitable<nlohmann::json> ListingRepository::fetchListings(const std::vector<record_t>& subscribedURLs) {
    json body = json::array();

    auto tx = Transaction(pgsql_n::Transaction());
    for (const auto& record: subscribedURLs) {
        const std::string& host = record.at("website_host").asString();
        const std::string& endpoint = record.at("website_endpoint").asString();

        json websiteListings;

        // 3.1) TODO: check in cache:
        //     3.1.1) TODO: if in cache and not expired, add it.

        const auto& [last_updated_at, listings] = fetchWebsiteListings(host, endpoint, tx);
        if (not listings.empty() and not last_updated_at.empty())
            websiteListings = listingsToJSON(host, last_updated_at, listings);
        else {
            const auto& [updatedTimestamp, listings] = co_await updateWebsiteListingsDatabase(host, endpoint, tx);
            websiteListings = listingsToJSON(host, updatedTimestamp, listings);
        }

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

    const auto request = http_n::Request().setMethod("GET")
                                          .setURL(host)
                                          .setAPIEndpoint(endpoint).build();

    co_return co_await http()->async_receive(co_await http()->async_send(request));
}

std::pair<std::string, std::vector<Listing>> ListingRepository::fetchWebsiteListings(std::string_view host, std::string_view endpoint, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    const auto websiteExistsQuery = Query().setTarget("websites")
                                           .setType(Query::Type_en::TARGETED)
                                           .setProjection({ "last_updated_at", "listings.*" })
                                           .setCardinality(Query::Cardinality_en::MULTIPLE)
                                           .setFilter({{ "host",      { "=", Value(host) }},
                                                       { "endpoint",  { "=", Value(endpoint) }},
                                                       { "expire_at", { "> now()", Value() }} })
                                           .addJoin({ Query::JoinType_en::INNER,
                                                      "listings",
                                                      std::nullopt,
                                                      std::nullopt,
                                                      {{ "websites.host",     "=", "listings.website_host" },
                                                       { "websites.endpoint", "=", "listings.website_endpoint" }}}).build();

    const Result& websiteExistsResult = database()->Read(websiteExistsQuery, &tx);

    if (not websiteExistsResult.isOK())
        throw Exception(websiteExistsResult.error().value());

    if (websiteExistsResult.isEmpty())
        return { "",  {} };

    const auto& recordsOpt = websiteExistsResult.records();
    std::vector<Listing> listings;
    for (const auto& record : recordsOpt.value())
        listings.emplace_back(Listing::fromDatabaseFormat(record));

    return { recordsOpt.value().at(0).at("last_updated_at").asString(), listings };
}

nlohmann::json ListingRepository::listingsToJSON(std::string_view host, std::string_view lastUpdated, const std::vector<Listing>& listings) const {
    json websiteListings;
    websiteListings["website_name"] = host;
    websiteListings["last_updated_at"] = Time::toSecondsSinceEpoch(Time::timepoint("%Y-%m-%d %H:%M:%S", lastUpdated));

    json listingsJSON = json::array();
    for (const auto& listing : listings)
        listingsJSON.push_back(listing.toJSON());

    websiteListings["listings"] = listingsJSON;
    return websiteListings;
}

asio::awaitable<std::pair<std::string, std::vector<Listing>>> ListingRepository::updateWebsiteListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx) {
    // TODO
    m_light->log(log_n::Level_en::WARNING, "Website", host, "listings are expired.");
    co_return std::make_pair("", std::vector<Listing>());
}