#pragma once


#include <core/time.hpp>
#include <core/str.hpp>
#include <database/utility/timestamp.hpp>
#include <database/value.hpp>
#include <chrono>
#include <nlohmann/json.hpp>


struct RecommendationTask {
    std::string uuid;
    std::string status;
    std::chrono::time_point<std::chrono::system_clock> started_at;
    std::chrono::time_point<std::chrono::system_clock> last_updated_at;

    nlohmann::json toJSON() const;

    static RecommendationTask fromDatabaseFormat(const database_n::record_t& record);
    static RecommendationTask fromJSON(const nlohmann::json& object);
    static std::vector<std::string> projection();
};