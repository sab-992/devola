#pragma once


#include <core/time.hpp>
#include <core/str.hpp>
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

    nlohmann::json databaseFormat() const;
    void normalize();

    void setAttribute(std::string_view attribute, auto value) {
        if (attribute == "id")
            id = std::stoll(value);
        else if (attribute == "website_id")
            website_id = std::stoll(value);
        else if (attribute == "title")
            title = value;
        else if (attribute == "category")
            category = value;
        else if (attribute == "company")
            company = value;
        else if (attribute == "location")
            location = value;
        else if (attribute == "publication")
            publication = std::chrono::floor<std::chrono::seconds>(Time::timepoint("%a, %d %b %Y %H:%M:%S %z", value));
        else if (attribute == "content")
            content = value;
        else if (attribute == "link")
            link = value;
    }
};