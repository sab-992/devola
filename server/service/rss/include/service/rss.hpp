#pragma once

#include <core/http.hpp>
#include <core/file.hpp>
#include <core/utility.hpp>
#include <database/postgre.hpp>
#include <listing/listing.hpp>
#include <parser/listings.hpp>
#include <utility/url.hpp>


class RSS : public http_n::server_n::Basic, public Service<RSS> {
    using Basic = http_n::server_n::Basic;
    using Session = http_n::server_n::Session;
    using json = nlohmann::json;
    using Database_i = database_n::Database_i;

public:
    RSS(const Private_s&, const json& configJSON);
    ~RSS();

    std::string pathPrefix() const override;
    void setCache(std::shared_ptr<Database_i> cache);
    void setDatabase(std::shared_ptr<Database_i> database);

    static std::unique_ptr<RSS> create(const json& configJSON);

private:
    std::shared_ptr<Database_i> m_cache;
    std::shared_ptr<Database_i> m_database;
    json m_configJSON;

    void setEndpoints();

    // TODO: Move functions below to its own class
    asio::awaitable<http_n::Response> fetchFeeds(const Session& session, const http_n::Request& request);
    asio::awaitable<http_n::Response> fetchFromURL(std::string_view host, std::string_view endpoint);
    std::pair<std::string, std::vector<Listing>> fetchListings(std::string_view host, std::string_view endpoint, Transaction& tx);
    bool isExpired(const std::chrono::time_point<std::chrono::system_clock>& expiryTimestamp) const;
    json listingsToJSON(std::string_view host, std::string_view lastUpdated, const std::vector<Listing>& listings) const;
    asio::awaitable<json> saveWebsiteListingsToDatabase(std::string_view host, std::string_view endpoint, Transaction& tx);
    asio::awaitable<json> updateWebsiteListingsDatabase(std::string_view host, std::string_view endpoint, Transaction& tx);
};