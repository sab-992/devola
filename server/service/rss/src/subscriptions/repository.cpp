#include <subscriptions/repository.hpp>


SubscriptionRepository::SubscriptionRepository(const ServerTools& tools) : m_tools(tools) {}

void SubscriptionRepository::createSubscriptions(std::string_view userUUID, const std::vector<std::string>& urls) {
    using namespace database_n;

    Transaction tx{ pgsql_n::Transaction() };
    for (const auto& url : urls) {
        const auto& [host, endpoint] = parseURL(url);
        const Result& result = database()->Create(Query().setTarget("subscriptions")
                                                         .setType(Query::Type_en::TARGETED)
                                                         .setCardinality(Query::Cardinality_en::NONE)
                                                         .setData({ { "user_uuid",        Value(userUUID) },
                                                                    { "website_host",     Value(host) },
                                                                    { "website_endpoint", Value(endpoint) } }).build(), &tx);
        if (not result.isOK()) {
            tx.get<pgsql_n::Transaction>().abort();
            throw Exception(result.error().value());
        }
    }
    tx.get<pgsql_n::Transaction>().commit();
}

std::shared_ptr<database_n::Database_i> SubscriptionRepository::database() {
    return m_tools.database;
}

std::vector<database_n::record_t> SubscriptionRepository::fetchSubscriptions(std::string_view userUUID) {
    using namespace database_n;

    const Result& result = database()->Read(Query().setTarget("subscriptions")
                                                   .setProjection({ "website_host", "website_endpoint" })
                                                   .setType(Query::Type_en::TARGETED)
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFilter({ { "user_uuid", { "=", userUUID } } }).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty())
        return {};

    return result.records().value();
}