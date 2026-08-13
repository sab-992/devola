#pragma once

#include <core/http.hpp>
#include <core/str.hpp>
#include <core/file.hpp>
#include <core/utility.hpp>
#include <dataclass/user.hpp>
#include <database/query.hpp>
#include <database/postgres.hpp>
#include <database/utility/timestamp.hpp>
#include <service/tools.hpp>


class UserRepository {
    using Basic = http_n::server_n::Basic;
    using Session = http_n::server_n::Session;
    using json = nlohmann::json;
    using Database_i = database_n::Database_i;
    using ServerTools = user::ServerTools;
    using record_t = database_n::record_t;

public:
    UserRepository(const ServerTools& tools);
    ~UserRepository() = default;

    friend std::unique_ptr<UserRepository> std::make_unique<UserRepository>();

    void createUser(const json& userInformation);
    std::unique_ptr<User> fetchUser(std::string_view username, std::string_view password);

private:
    ServerTools m_tools;
    std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();

    std::shared_ptr<Database_i> database();
    database_n::record_t RecordFromJSON(const json& userInformation);
};