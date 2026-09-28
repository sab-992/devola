#pragma once

#include <nlohmann/json.hpp>
#include <service/tools.hpp>
#include <utility/url.hpp>


class SubscriptionRepository {
    using json = nlohmann::json;
    using record_t = database_n::record_t;
    using ServerTools = rss::ServerTools;

public:
    SubscriptionRepository(const ServerTools& tools);
    ~SubscriptionRepository() = default;

    friend std::unique_ptr<SubscriptionRepository> std::make_unique<SubscriptionRepository>();

    void saveSubscriptions(std::string_view userUUID, const json& subscriptions, Transaction& tx);
    std::vector<record_t> fetchSubscriptions(std::string_view userUUID);

private:
    ServerTools m_tools;

    std::shared_ptr<database_n::Database_i> database();
};