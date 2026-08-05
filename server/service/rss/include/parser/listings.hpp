#pragma once

#include <core/exception.hpp>
#include <core/str.hpp>
#include <core/time.hpp>
#include <core/xml.hpp>
#include <core/utility.hpp>
#include <database/value.hpp>
#include <listing/listing.hpp>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>


namespace parser_n
{
    class Listings {
        using xml_node = pugi::xml_node;
        using record_t = database_n::record_t;
        using listings_t = std::vector<record_t>;
        using aliases_t = std::vector<std::string>;
        template <typename T>
        using hashmap_t = std::unordered_map<std::string, T>;

        inline static const std::unordered_set<std::string> TTL_ALIASES = { "ttl", "sy:updateFrequency" };
        inline static const hashmap_t<aliases_t> ITEM_NODES_ALIASES = {{ "title",       { "title" } },
                                                                       { "category",    { "category" } },
                                                                       { "company",     { "companyName", "company" } },
                                                                       { "location",    { "location", "region" } },
                                                                       { "publication", { "pubDate", "published", "dc:date", "updated" } },
                                                                       { "content",     { "content:encoded", "description", "content", "summary" } },
                                                                       { "link",        { "link" } }};

        inline static const std::unordered_set<std::string> CONTENT_TO_VERIFY = { "title", "company", "location", "content", "link" };

        inline static constexpr size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

    public:
        Listings() = delete;
        ~Listings() = default;

        static std::vector<Listing> parse(int64_t websiteID, const xml_n::Document& document);

    private:

        static bool addRelevantNodes(xml_node& node, Listing& listing, const hashmap_t<aliases_t>& aliases={}) {
            for (const auto& [key, candidates] : aliases) {
                std::string value = search(node, candidates);

                // Remove the illegal and fake state
                if (CONTENT_TO_VERIFY.contains(key) and toLower(value).find("israel") != std::string::npos)
                    return false;

                listing.setAttribute(key, value);
            }

            listing.normalize();
            return true;
        }

        static std::chrono::time_point<std::chrono::system_clock> computeExpirationTimepoint(int ttl);
        static int parseTTL(const xml_node& ttl);
        static size_t relevancy(std::string_view name, const aliases_t& aliases, const size_t currentBest);
        static std::string search(xml_node& node, const aliases_t& aliases);
    };
}