#include <subscriptions/repository.hpp>


SubscriptionRepository::SubscriptionRepository(const ServerTools& tools) : m_tools(tools) {}

void SubscriptionRepository::createSubscriptions(std::string_view userUUID, const std::vector<std::string>& urls) {}

std::vector<database_n::record_t> SubscriptionRepository::fetchSubscriptions(std::string_view userUUID) {
    // 2) TODO: Fetch subscribed urls from DB.
    return { {{ "website_host", "weworkremotely.com" }, { "website_endpoint", "/categories/remote-customer-support-jobs.rss" }},
             {{ "website_host", "remotive.com"},        { "website_endpoint", "/remote-jobs/feed/software-development" }},
             {{ "website_host",  "himalayas.app"},      { "website_endpoint", "/jobs/rss"}},
             {{ "website_host", "jobicy.com"},          { "website_endpoint", "/jobs/feed?industry=engineering"}} };
}