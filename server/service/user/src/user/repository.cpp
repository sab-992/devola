#include <user/repository.hpp>


UserRepository::UserRepository(const ServerTools& tools) : m_tools(tools) {
    m_crypto = Cryptography::instance();
}


void UserRepository::createUser(const json& userInformation) {
    using namespace database_n;

    const Result& result = database()->Create(Query().setTarget("users")
                                                     .setType(Query::Type_en::TARGETED)
                                                     .setCardinality(Query::Cardinality_en::NONE)
                                                     .setData(RecordFromJSON(userInformation)).build());

    if (not result.isOK())
        throw Exception(result.error().value());
}

std::shared_ptr<database_n::Database_i> UserRepository::database() {
    return m_tools.database;
}

std::unique_ptr<User> UserRepository::fetchUser(std::string_view username, std::string_view password) {
    using namespace database_n;

    auto projection = User::projection();
    projection.emplace_back("password");
    const Result& result = database()->Read(Query().setTarget("users")
                                                   .setProjection(projection)
                                                   .setType(Query::Type_en::TARGETED)
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFilter({{ "username", { "=", username } }}).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty() or result.size() != 1)
        return nullptr;

    const auto& recordsOPT = result.records();
    const auto& record = recordsOPT.value().at(0);
    if (not record.contains("password"))
        throw Exception("No hashed password in the user record");

    m_crypto->verifyHash(record.at("password").asString(), password);

    return std::make_unique<User>(User::fromDatabaseFormat(record));
}

std::unique_ptr<User> UserRepository::fetchUserByUUID(std::string_view userUUID) {
    using namespace database_n;

    const Result& result = database()->Read(Query().setTarget("users")
                                                   .setProjection(User::projection())
                                                   .setType(Query::Type_en::TARGETED)
                                                   .setCardinality(Query::Cardinality_en::MULTIPLE)
                                                   .setFilter({{ "uuid", { "=", userUUID } }}).build());

    if (not result.isOK())
        throw Exception(result.error().value());
    else if (result.isEmpty() or result.size() != 1)
        return nullptr;

    return std::make_unique<User>(User::fromDatabaseFormat(result.records()->at(0)));
}

database_n::record_t UserRepository::RecordFromJSON(const json& userInformation) {
    record_t userRecord;
    for (auto& [key, value] : userInformation.items()) {
        if (value.is_null() or not value.is_string())
            continue;

        if (key == "password")
            userRecord[key] = m_crypto->hash(Argon2id::instance(), value.get<std::string>());
        else
            userRecord[key] = value.get<std::string>();
    }
    return userRecord;
}