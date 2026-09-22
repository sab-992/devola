#pragma once

#include <core/http.hpp>
#include <core/str.hpp>
#include <core/file.hpp>
#include <core/utility.hpp>
#include <database/postgres.hpp>
#include <database/utility/timestamp.hpp>
#include <dataclass/listing.hpp>
#include <parser/listings.hpp>
#include <service/tools.hpp>
#include <utility/url.hpp>


class ListingRepository {
    using Basic = http_n::server_n::Basic;
    using Session = http_n::server_n::Session;
    using json = nlohmann::json;
    using Database_i = database_n::Database_i;
    using ServerTools = rss::ServerTools;
    using record_t = database_n::record_t;

public:
    ListingRepository(const ServerTools& tools);
    ~ListingRepository() = default;

    friend std::unique_ptr<ListingRepository> std::make_unique<ListingRepository>();

    void createWebsites(const json& subscriptions, Transaction& tx);
    asio::awaitable<json> fetchListings(const std::vector<record_t>& subscriptions);

private:
    ServerTools m_tools;
    std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();

    std::shared_ptr<Database_i> cache();
    std::shared_ptr<Database_i> database();

    json buildPayload(std::string_view host, std::string_view lastUpdated, const json& listingsJSON) const;
    json buildPayload(std::string_view host, std::string_view lastUpdated, const std::vector<Listing>& listings) const;
    json createListings(std::string_view host, std::string_view endpoint, const std::vector<Listing>& listings, Transaction& tx);
    asio::awaitable<http_n::Response> fetchFromURL(std::string_view host, std::string_view endpoint);
    std::pair<std::string, std::vector<Listing>> fetchWebsiteListings(std::string_view host, std::string_view endpoint, Transaction& tx);
    const std::unique_ptr<http_n::Http>& http();
    json listingsToJSON(const std::vector<Listing>& listings) const;
    void saveWebsiteListingsToCache(const std::string& target, const json& websiteListings);
    asio::awaitable<json> updateWebsiteListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx);
};