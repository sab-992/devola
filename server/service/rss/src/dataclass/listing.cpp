#include <dataclass/listing.hpp>


Listing Listing::fromDatabaseFormat(const database_n::record_t& record) {
    return { record.at("id").asInt64(),
             record.at("website_host").asString(),
             record.at("website_endpoint").asString(),
             record.at("title").asString(),
             record.at("category").asString(),
             record.at("company").asString(),
             record.at("location").asString(),
             record.at("content").asString(),
             record.at("link").asString(),
             fromPGSQLFormat(record.at("created_at").asString()),
             fromPGSQLFormat(record.at("expire_at").asString()) };
}

std::chrono::time_point<std::chrono::system_clock> Listing::parseDate(std::string_view date) const {
    if (date.size() >= 3 and date.compare(date.size() - 3, 3, "GMT") == 0)
        return Time::timepoint("%a, %d %b %Y %H:%M:%S GMT", date);
    else
        return Time::timepoint("%a, %d %b %Y %H:%M:%S %z", date);

    throw Exception(std::format("Failed to parse listing date: {}", date));
}

void Listing::setAttribute(std::string_view attribute, std::string_view value) {
    if (attribute == "id")
        id = std::stoll(std::string(value));
    else if (attribute == "website_host")
        website_host = value;
    else if (attribute == "website_endpoint")
        website_endpoint = value;
    else if (attribute == "title")
        title = value;
    else if (attribute == "category")
        category = value;
    else if (attribute == "company")
        company = value;
    else if (attribute == "location")
        location = value;
    else if (attribute == "content")
        content = value;
    else if (attribute == "link")
        link = value;
    else if (attribute == "created_at")
        created_at = parseDate(value);
    else if (attribute == "expire_at")
        expire_at = parseDate(value);
}

void Listing::normalize() {
    if (not trim(company).empty())
        return;

    company = title.substr(0, title.find(":"));
}

nlohmann::json Listing::toJSON() const {
    auto object = nlohmann::json({ { "id",               id },
                                   { "website_host",     website_host },
                                   { "website_endpoint", website_endpoint },
                                   { "title",            title },
                                   { "category",         category },
                                   { "company",          company },
                                   { "location",         location },
                                   { "content",          content },
                                   { "link",             link },
                                   { "created_at",       Time::toSecondsSinceEpoch(created_at) },
                                   { "expire_at",        Time::toSecondsSinceEpoch(expire_at) }});

    return object;
}