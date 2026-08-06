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