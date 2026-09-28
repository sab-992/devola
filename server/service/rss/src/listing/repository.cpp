#include <listing/repository.hpp>


ListingRepository::ListingRepository(const ServerTools& tools) : m_tools(tools) {}

nlohmann::json ListingRepository::buildPayload(std::string_view host, std::string_view lastUpdated, const json& listingsJSON) const {
    return json({{ "website_name", host },
                 { "listings", listingsJSON },
                 { "last_updated_at", Time::toSecondsSinceEpoch(Time::timepoint("%Y-%m-%d %H:%M:%S", lastUpdated)) }});
}

nlohmann::json ListingRepository::buildPayload(std::string_view host, std::string_view lastUpdated, const std::vector<Listing>& listings) const {
    return buildPayload(host, lastUpdated, listingsToJSON(listings));
}

std::shared_ptr<database_n::Database_i> ListingRepository::cache() {
    return m_tools.cache;
}

nlohmann::json ListingRepository::createListings(std::string_view host, std::string_view endpoint, const std::vector<Listing>& listings, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    const Result& result = database()->Other(Query().setTarget("insert_listings_batch")
                                                    .setType(Query::Type_en::PROCEDURE)
                                                    .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                    .setFunctionData({ listingsToJSON(listings).dump(), Value() }).build(), &tx);

    if (not result.isOK())
        throw Exception(result.error().value());

    if (result.isEmpty() or result.size() != 1)
        throw Exception(std::format("Empty results - {}", FUNCTION_SIGNATURE));

    return json::parse(result.records().value().at(0).at("result").asString());
}

void ListingRepository::createWebsites(const json& subscriptions, Transaction& tx) {
    using namespace http_n;
    using namespace database_n;

    for (const auto& websiteJSON : subscriptions) {
        const std::string& host = websiteJSON["host"].get<std::string>();
        const std::string& endpoint = websiteJSON["endpoint"].get<std::string>();

        constexpr size_t MAX_WEBSITE_LEN  = 253;
        if (host.size() > MAX_WEBSITE_LEN)
            throw InvalidArgument(std::format("exceeds maximum size of {} characters", MAX_WEBSITE_LEN), "Hostname");

        constexpr size_t MAX_ENDPOINT_LEN = 255;
        if (endpoint.size() > MAX_ENDPOINT_LEN)
            throw InvalidArgument(std::format("exceeds maximum size of {} characters", MAX_ENDPOINT_LEN), "Endpoint");
    }

    const Result& result = database()->Other(Query().setTarget("insert_websites")
                                                    .setType(Query::Type_en::PROCEDURE)
                                                    .setCardinality(Query::Cardinality_en::NONE)
                                                    .setFunctionData({ subscriptions.dump() }).build(), &tx);

    if (not result.isOK())
        throw Exception(result.error().value());
}

std::shared_ptr<database_n::Database_i> ListingRepository::database() {
    return m_tools.database;
}

const std::unique_ptr<http_n::Http>& ListingRepository::http() {
    return m_tools.http;
}

asio::awaitable<nlohmann::json> ListingRepository::fetchListings(const std::vector<record_t>& subscriptions) {
    using namespace database_n;
    json body = json::array();

    auto tx = Transaction(pgsql_n::Transaction());
    bool updatedWebsiteListings = false;
    for (const auto& record: subscriptions) {
        const std::string& host = record.at("website_host").asString();
        const std::string& endpoint = record.at("website_endpoint").asString();

        const std::string& cacheTarget = std::format("{}{}", host, endpoint);
        const Result& result = cache()->Read(Query().setTarget(cacheTarget)
                                                    .setType(Query::Type_en::TARGETED)
                                                    .setCardinality(Query::Cardinality_en::MULTIPLE).build());

        if (not result.isOK())
            throw Exception(result.error().value());

        json websiteListings;
        if (not result.isEmpty() and
            result.records().value().at(0).contains(cacheTarget) and
            not result.records().value().at(0).at(cacheTarget).isNull())
            websiteListings = json::parse(result.records().value().at(0).at(cacheTarget).asString());
        else {
            updatedWebsiteListings = true;
            const auto& [last_updated_at, listings] = fetchWebsiteListings(host, endpoint, tx);
            if (not listings.empty() and not last_updated_at.empty())
                websiteListings = buildPayload(host, last_updated_at, listings);
            else
                websiteListings = co_await updateWebsiteListingsDatabase(host, endpoint, tx);

            saveWebsiteListingsToCache(cacheTarget, websiteListings);
        }

        body.emplace_back(websiteListings);
    }

    if (updatedWebsiteListings)
        tx.get<pgsql_n::Transaction>().commit();

    co_return body;
}

asio::awaitable<http_n::Response> ListingRepository::fetchFromURL(std::string_view host, std::string_view endpoint) {
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

nlohmann::json ListingRepository::listingsToJSON(const std::vector<Listing>& listings) const {
    json listingsJSON = json::array();
    for (const auto& listing : listings)
        listingsJSON.emplace_back(listing.toJSON());
    return listingsJSON;
}

void ListingRepository::saveWebsiteListingsToCache(const std::string& target, const json& websiteListings) {
    using namespace database_n;

    Query::Options opt;
    if (not websiteListings.empty() and
            websiteListings.is_object() and
            websiteListings["listings"].is_array() and
            websiteListings["listings"].size() > 0)
        opt.ttl = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>{websiteListings["listings"][0]["expire_at"].get<double>() - websiteListings["listings"][0]["created_at"].get<double>()});

    const Result& result = cache()->Create(Query().setTarget(target)
                                                  .setType(Query::Type_en::TARGETED)
                                                  .setCardinality(Query::Cardinality_en::NONE)
                                                  .setData({{ target, websiteListings.dump() }})
                                                  .setOptions(opt).build());

    if (not result.isOK())
        throw Exception(result.error().value());
}

asio::awaitable<nlohmann::json> ListingRepository::updateWebsiteListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx) {
    using namespace database_n;

    const Result& updateWebsiteResult = database()->Update(Query().setTarget("websites")
                                                                  .setType(Query::Type_en::TARGETED)
                                                                  .setProjection({ "last_updated_at" })
                                                                  .setCardinality(Query::Cardinality_en::SINGLE)
                                                                  .setData({{ "last_updated_at", Value(toPGSQLFormat(Time::now())) }})
                                                                  .setFilter({ { "host",     { "=", Value(host) }},
                                                                               { "endpoint", { "=", Value(endpoint) }} }).build());

    const http_n::Response& feed = co_await fetchFromURL(host, endpoint);
    co_return buildPayload(host,
                           updateWebsiteResult.records()->at(0).at("last_updated_at").asString(),
                           createListings(host, endpoint,
                                          parser_n::Listings::parse(host, endpoint, feed.body<xml_n::Document>()), tx));
}