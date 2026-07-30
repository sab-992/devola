#pragma once

#include <climits>
#include <core/exception.hpp>
#include <core/str.hpp>
#include <core/xml.hpp>
#include <core/utility.hpp>
#include <database/value.hpp>
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
        using websiteInfo_t = hashmap_t<std::string>;

        inline static const std::unordered_set<std::string> TTL_ALIASES = { "ttl", "sy:updateFrequency" };
        inline static const hashmap_t<aliases_t> ITEM_NODES_ALIASES = {{ "title",       { "title" } },
                                                                       { "category",    { "category" } },
                                                                       { "company",     { "companyName", "company" } },
                                                                       { "location",    { "location", "region" } },
                                                                       { "publication", { "pubDate", "published", "dc:date", "updated" } },
                                                                       { "content",     { "content:encoded", "description", "content", "summary" } },
                                                                       { "link",        { "link" } }};

        inline static const std::unordered_set<std::string> TO_VERIFY_CONTENT = { "title", "company", "location", "content", "link" };

        inline static constexpr size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

    public:
        Listings() = delete;
        ~Listings() = default;

        static std::tuple<int, websiteInfo_t, std::vector<record_t>> parse(const xml_n::Document& document);

    private:

        template <typename T>
            requires (std::is_convertible_v<T, database_n::Value> || std::is_convertible_v<T, std::string>)
        static hashmap_t<T> addRelevantNodes(xml_node& node, const hashmap_t<aliases_t>& aliases={}) {
            hashmap_t<T> map;

            for (const auto& [key, candidates] : aliases) {
                std::string value = search(node, candidates);

                // Remove the illegal and fake state
                if (TO_VERIFY_CONTENT.contains(key) and toLower(value).find("israel") != std::string::npos) {
                    map.clear();
                    return std::move(map);
                }

                map.emplace(key, value);
            }

            if constexpr (std::is_same_v<T, database_n::Value>)
                normalize(map);

            return std::move(map);
        }

        static void normalize(record_t& record);
        static int parseTTL(const xml_node& ttl);
        static bool recordContains(const record_t& record, const std::string& key);
        static size_t relevancy(std::string_view name, const aliases_t& aliases, const size_t currentBest);
        static std::string search(xml_node& node, const aliases_t& aliases);
    };
}