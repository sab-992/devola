#include <listing/listing.hpp>


nlohmann::json Listing::databaseFormat() const {
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

void Listing::normalize() {
    if (not trim(company).empty())
        return;

    company = title.substr(0, title.find(":"));
}

void Listing::setAttribute(std::string_view attribute, auto value) {
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