#include <listing/listing.hpp>


std::chrono::time_point<std::chrono::system_clock> Listing::format(const std::string& format, std::string_view value) {
    return std::chrono::floor<std::chrono::seconds>(Time::timepoint(format, value));
}

Listing Listing::fromDatabaseFormat(const database_n::record_t& record) {
    return { record.at("id").asInt64(),
             record.at("website_id").asInt64(),
             record.at("title").asString(),
             record.at("category").asString(),
             record.at("company").asString(),
             record.at("location").asString(),
             format("%Y-%m-%d %H:%M:%S", record.at("publication").asString()),
             record.at("content").asString(),
             record.at("link").asString(),
             format("%Y-%m-%d %H:%M:%S", record.at("expire_at").asString()) };
}

void Listing::setAttribute(std::string_view attribute, std::string_view value) {
    if (attribute == "id")
        id = std::stoll(std::string(value));
    else if (attribute == "website_id")
        website_id = std::stoll(std::string(value));
    else if (attribute == "title")
        title = value;
    else if (attribute == "category")
        category = value;
    else if (attribute == "company")
        company = value;
    else if (attribute == "location")
        location = value;
    else if (attribute == "publication")
        publication = format("%a, %d %b %Y %H:%M:%S %z", value);
    else if (attribute == "content")
        content = value;
    else if (attribute == "link")
        link = value;
}

void Listing::normalize() {
    if (not trim(company).empty())
        return;

    company = title.substr(0, title.find(":"));
}

nlohmann::json Listing::ToDatabaseFormat() const {
    auto object = nlohmann::json({ { "website_id",  website_id },
                                   { "title",       title },
                                   { "category",    category },
                                   { "company",     company },
                                   { "location",    location },
                                   { "publication", Time::convertToSecondsSinceEpoch(std::chrono::floor<std::chrono::seconds>(publication)) },
                                   { "content",     content },
                                   { "link",        link },
                                   { "expire_at",   Time::convertToSecondsSinceEpoch(std::chrono::floor<std::chrono::seconds>(expire_at)) } });

    if (id.has_value() and id.value() >= 0)
        object["id"] = id;

    return object;
}