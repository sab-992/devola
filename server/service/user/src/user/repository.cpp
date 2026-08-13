#include <user/repository.hpp>


UserRepository::UserRepository(const ServerTools& tools) : m_tools(tools) {}


void UserRepository::createUser(const json& userInformation) {
    using namespace database_n;

    const auto createUserQuery = Query().setTarget("users")
                                        .setType(Query::Type_en::TARGETED)
                                        .setCardinality(Query::Cardinality_en::NONE)
                                        .setData(RecordFromJSON(userInformation)).build();

    const Result& result = database()->Create(createUserQuery);

    if (not result.isOK())
        throw Exception(result.error().value());
}

std::shared_ptr<database_n::Database_i> UserRepository::database() {
    return m_tools.database;
}

std::unique_ptr<User> UserRepository::fetchUser(std::string_view username, std::string_view password) {
    using namespace database_n;

    // TODO hash password
    const auto fetchUserQuery = Query().setTarget("users")
                                       .setProjection(User::projection())
                                       .setType(Query::Type_en::TARGETED)
                                       .setCardinality(Query::Cardinality_en::MULTIPLE)
                                       .setFilter({ { "username", { "=", username } },
                                                    { "password", { "=", password } } }).build();

    const Result& result = database()->Read(fetchUserQuery);

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

        userRecord[key] = value.get<std::string>();
    }
    return userRecord;
}