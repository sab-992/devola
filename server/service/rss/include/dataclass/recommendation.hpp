#pragma once


#include <core/time.hpp>
#include <core/str.hpp>
#include <database/utility/timestamp.hpp>
#include <database/value.hpp>
#include <chrono>
#include <nlohmann/json.hpp>


struct Recommendation {
    std::string task_uuid;
    std::string status;
    std::string website_host;
    std::string website_endpoint;
    std::chrono::time_point<std::chrono::system_clock> started_at;
    std::chrono::time_point<std::chrono::system_clock> last_updated_at;

    nlohmann::json toJSON() const;

    static Recommendation fromDatabaseFormat(const database_n::record_t& record);
    static Recommendation fromJSON(const nlohmann::json& object);
    static std::vector<std::string> projection();
};