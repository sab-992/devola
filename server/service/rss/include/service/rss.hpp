#pragma once

#include <core/http.hpp>
#include <core/file.hpp>
#include <core/jwt.hpp>
#include <core/utility.hpp>
#include <database/postgres.hpp>
#include <database/redis.hpp>
#include <dataclass/listing.hpp>
#include <listing/repository.hpp>
#include <recommendation/repository.hpp>
#include <resume/repository.hpp>
#include <subscriptions/repository.hpp>
#include <parser/listings.hpp>
#include <service/tools.hpp>
#include <utility/url.hpp>


class RSSService : public http_n::server_n::Basic, public Service<RSSService> {
    using Basic = http_n::server_n::Basic;
    using Code = network_n::Code;
    using Database_i = database_n::Database_i;
    using json = nlohmann::json;
    using Response = http_n::Response;
    using Session = http_n::server_n::Session;

public:
    RSSService(const Private_s&, const json& configJSON);
    ~RSSService();

    std::string pathPrefix() const override;
    void setCache(std::shared_ptr<Database_i> cache);
    void setDatabase(std::shared_ptr<Database_i> database);

    static std::unique_ptr<RSSService> create(const json& configJSON);

private:
    std::shared_ptr<Database_i> m_cache;
    json m_configJSON;
    std::shared_ptr<Database_i> m_database;
    std::unique_ptr<ListingRepository> m_listingRepos;
    std::shared_ptr<RecommendationRepository> m_recommendationRepos;
    std::unique_ptr<ResumeRepository> m_resumeRepos;
    std::unique_ptr<SubscriptionRepository> m_subscriptionRepos;

    asio::awaitable<http_n::Response> addResume(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> deleteResume(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> feeds(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> recommend(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> recommendation(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> recommendations(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> resumes(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> subscribe(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> subscriptions(const Session& session, const http_n::Request& request, const pathParams_t& params);
    asio::awaitable<http_n::Response> updateResume(const Session& session, const http_n::Request& request, const pathParams_t& params);

    void setEndpoints();
    rss::ServerTools tools();
};