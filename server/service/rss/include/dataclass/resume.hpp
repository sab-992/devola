#pragma once


#include <core/time.hpp>
#include <core/str.hpp>
#include <database/utility/timestamp.hpp>
#include <database/value.hpp>
#include <chrono>
#include <nlohmann/json.hpp>
#include <vector>


struct Resume {
    std::string tag;
    std::string content;
    std::vector<std::string> skills;
    std::chrono::time_point<std::chrono::system_clock> last_updated_at;

    nlohmann::json toJSON() const;
    database_n::record_t toDatabaseFormat() const;

    static Resume fromDatabaseFormat(const database_n::record_t& record);
    static Resume fromJSON(const nlohmann::json& object);
    static std::vector<std::string> projection();
};