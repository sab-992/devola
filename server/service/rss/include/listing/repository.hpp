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

    asio::awaitable<void> createWebsiteListings(const std::vector<std::string>& urls);
    asio::awaitable<json> fetchListings(const std::vector<record_t>& subscribedURLs);

private:
    ServerTools m_tools;
    std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();

    std::shared_ptr<Database_i> cache();
    asio::awaitable<void> createListings(std::string_view host, std::string_view endpoint, std::string_view last_updated_at, Transaction& tx);
    std::string createWebsite(std::string_view host, std::string_view endpoint, Transaction& tx);
    std::shared_ptr<Database_i> database();
    asio::awaitable<http_n::Response> fetchFromURL(std::string_view host, std::string_view endpoint);
    std::pair<std::string, std::vector<Listing>> fetchWebsiteListings(std::string_view host, std::string_view endpoint, Transaction& tx);
    const std::unique_ptr<http_n::Http>& http();
    json listingsToJSON(std::string_view host, std::string_view lastUpdated, const std::vector<Listing>& listings) const;
    asio::awaitable<std::pair<std::string, std::vector<Listing>>> updateWebsiteListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx);
};