#include <subscriptions/repository.hpp>


SubscriptionRepository::SubscriptionRepository(const ServerTools& tools) : m_tools(tools) {}

void SubscriptionRepository::saveSubscriptions(std::string_view userUUID, const json& subscriptions, Transaction& tx) {
    using namespace database_n;

    const Result& result = database()->Other(Query().setTarget("save_subscriptions")
                                                    .setType(Query::Type_en::PROCEDURE)
                                                    .setCardinality(Query::Cardinality_en::NONE)
                                                    .setFunctionData({ userUUID, subscriptions.dump() }).build(), &tx);
    if (not result.isOK())
        throw Exception(result.error().value());
}

std::shared_ptr<database_n::Database_i> SubscriptionRepository::database() {
    return m_tools.database;
}

std::vector<database_n::record_t> SubscriptionRepository::fetchSubscriptions(std::string_view userUUID) {
    using namespace database_n;

    const Result& result = database()->Read(Query().setTarget("subscriptions")
                                                   .setProjection({ "website_host", "website_endpoint", "created_at" })
                                                   .setType(Query::Type_en::TARGETED)
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFilter({ { "user_uuid", { "=", userUUID } } }).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty())
        return {};

    return result.records().value();
}