#pragma once

#include <core/http.hpp>
#include <core/file.hpp>
#include <core/utility.hpp>
#include <database/postgres.hpp>
#include <listing/listing.hpp>
#include <listing/repository.hpp>
#include <parser/listings.hpp>
#include <service/tools.hpp>
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
    json m_configJSON;
    std::shared_ptr<Database_i> m_database;
    std::unique_ptr<ListingRepository> m_listingRepos;

    asio::awaitable<http_n::Response> fetchFeeds(const Session& session, const http_n::Request& request);
    asio::awaitable<http_n::Response> recommend(const Session& session, const http_n::Request& request);
    asio::awaitable<http_n::Response> recommendations(const Session& session, const http_n::Request& request);
    void setEndpoints();
    rss::ServerTools tools();

};