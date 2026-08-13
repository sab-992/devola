#pragma once


#include <core/time.hpp>
#include <core/str.hpp>
#include <database/utility/timestamp.hpp>
#include <database/value.hpp>
#include <chrono>
#include <nlohmann/json.hpp>


struct Resume {
    std::string tag;
    std::string content;
    std::chrono::time_point<std::chrono::system_clock> created_at;
    std::chrono::time_point<std::chrono::system_clock> last_updated_at;

    nlohmann::json toJSON() const;

    static Resume fromDatabaseFormat(const database_n::record_t& record);
    static Resume fromJSON(const nlohmann::json& object);
    static std::vector<std::string> projection();
};