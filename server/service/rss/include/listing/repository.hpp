#pragma once

#include <core/http.hpp>
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

public:
    ListingRepository(const ServerTools& tools);
    ~ListingRepository() = default;

    friend std::unique_ptr<ListingRepository> std::make_unique<ListingRepository>();

    asio::awaitable<json> fetchListings(const std::vector<std::string>& subscribedURLs);

private:
    ServerTools m_tools;
    std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();

    std::shared_ptr<Database_i> cache();
    std::shared_ptr<Database_i> database();
    asio::awaitable<http_n::Response> fetchFromURL(std::string_view host, std::string_view endpoint);
    std::tuple<std::string, std::string, std::vector<Listing>> fetchWebsiteListings(std::string_view host, std::string_view endpoint, Transaction& tx);
    const std::unique_ptr<http_n::Http>& http();
    bool isExpired(const std::chrono::time_point<std::chrono::system_clock>& expiryTimestamp) const;
    json listingsToJSON(std::string_view host, std::string_view lastUpdated, std::string_view expireAt, const std::vector<Listing>& listings) const;
    asio::awaitable<json> saveWebsiteListingsToDatabase(std::string_view host, std::string_view endpoint, Transaction& tx);
    asio::awaitable<json> updateWebsiteListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx);
};