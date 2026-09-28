#include <token/repository.hpp>


TokenRepository::TokenRepository(const ServerTools& tools) : m_tools(tools) {}

void TokenRepository::createRefreshToken(const RefreshToken& token, std::string_view userUUID) {
    using namespace database_n;

    const Result& result = database()->Create(Query().setTarget("refresh_tokens")
                                                     .setType(Query::Type_en::TARGETED)
                                                     .setCardinality(Query::Cardinality_en::NONE)
                                                     .setData(token.toDatabaseFormat()).build());

    if (not result.isOK())
        throw Exception(result.error().value());
}

std::string TokenRepository::validateRefreshToken(std::string_view token) {
    using namespace database_n;

    const std::string& hashedToken = m_crypto->hash(Generic::instance(), token);
    const Result& result = database()->Read(Query().setTarget("refresh_tokens")
                                                   .setProjection({ "user_uuid" })
                                                   .setType(Query::Type_en::TARGETED)
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFilter({{ "token",     { "=", hashedToken } },
                                                               { "expire_at", { "> now()", Value() }},
                                                               { "revoked",   { "=", false }} }).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty() or result.size() != 1)
        throw InvalidToken("No valid refresh tokens found");

    return result.records().value().at(0).at("user_uuid").asString();
}

void TokenRepository::revokeRefreshToken(std::string_view token) {
    using namespace database_n;

    const std::string& hashedToken = m_crypto->hash(Generic::instance(), token);
    const Result& result = database()->Update(Query().setTarget("refresh_tokens")
                                                     .setType(Query::Type_en::TARGETED)
                                                     .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                     .setData({{ "revoked", true }})
                                                     .setFilter({{ "token", { "=", hashedToken }}}).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (not result.isEmpty())
        throw Exception(std::format("Result is not empty ({} elements) while revoking refresh token", result.size()));
}

std::shared_ptr<database_n::Database_i> TokenRepository::database() {
    return m_tools.database;
}