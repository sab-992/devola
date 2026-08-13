#pragma once

#include <service/tools.hpp>


class SubscriptionRepository {
    using record_t = database_n::record_t;
    using ServerTools = rss::ServerTools;

public:
    SubscriptionRepository(const ServerTools& tools);
    ~SubscriptionRepository() = default;

    friend std::unique_ptr<SubscriptionRepository> std::make_unique<SubscriptionRepository>();

    void createSubscriptions(std::string_view userUUID, const std::vector<std::string>& urls);
    std::vector<record_t> fetchSubscriptions(std::string_view userUUID);

private:
    ServerTools m_tools;
};