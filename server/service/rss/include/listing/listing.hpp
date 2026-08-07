#pragma once


#include <core/time.hpp>
#include <core/str.hpp>
#include <database/value.hpp>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>


struct Listing {
    std::optional<int64_t> id = -1;
    int64_t website_id;
    std::string title;
    std::string category;
    std::string company;
    std::string location;
    std::chrono::time_point<std::chrono::system_clock> publication;
    std::string content;
    std::string link;
    std::chrono::time_point<std::chrono::system_clock> expire_at;

    void normalize();
    void setAttribute(std::string_view attribute, std::string_view value);
    nlohmann::json ToDatabaseFormat() const;

    static Listing fromDatabaseFormat(const database_n::record_t& record);

private:
    static std::chrono::time_point<std::chrono::system_clock> format(const std::string& format, std::string_view value);
};