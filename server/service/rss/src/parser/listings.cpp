#include <parser/listings.hpp>


std::chrono::time_point<std::chrono::system_clock> parser_n::Listings::computeExpirationTimepoint(int ttl){
    return Time::now() + std::chrono::minutes(ttl);
}

std::vector<Listing> parser_n::Listings::parse(int64_t websiteID, const xml_n::Document& document) {
    xml_node channelNode = document.find_node([](const xml_node& node) -> bool { return strcmp(node.name(), "channel") == 0; });

    if (not channelNode)
        throw InvalidArgument("Invalid XML format", "XML document");

    unsigned int ttl = UINT_MAX;
    std::vector<Listing> listings;

    for (xml_node node = channelNode.first_child(); node; node = node.next_sibling()) {
        std::string name = node.name();

        if (TTL_ALIASES.contains(name))
            ttl = parseTTL(node);
        else if (name == "item") {
            Listing listing;

            // Ignore if no relevant nodes are added (aka Listing is empty).
            if (not addRelevantNodes(node, listing, ITEM_NODES_ALIASES))
                continue;

            listing.website_id = websiteID;
            listing.expire_at = computeExpirationTimepoint(ttl);
            listings.emplace_back(listing);
        }
    }

    return listings;
}

int parser_n::Listings::parseTTL(const xml_node& node) {
    const uint8_t TTL_INCREASE = 5;
    const int MAX_TTL = 1440;
    int ttl = std::stoi(node.first_child().value());

    auto normalize = [](int result) -> int {
        int normalized = result * TTL_INCREASE;
        return (normalized > MAX_TTL) ? MAX_TTL : normalized;
    };

    // Convert hours into minutes.
    return normalize(ttl > 24 ? ttl : ttl * 60);
}

size_t parser_n::Listings::relevancy(std::string_view name, const aliases_t& aliases, const size_t currentBest) {
    std::function<std::pair<bool, std::string>(const std::string&)> truthy = [](const std::string& key) -> std::pair<bool, std::string> { return { true, key }; };

    size_t i = 0;
    for (; i < aliases.size(); i++) {
        const std::string& candidate = aliases[i];

        if (name == candidate)
            return i;

        const size_t NAMESPACE_SEPARATOR_INDEX = name.find(":");
        if (NAMESPACE_SEPARATOR_INDEX != std::string::npos) {
            bool isInsideNamespace = name.substr(NAMESPACE_SEPARATOR_INDEX + 1) == candidate;
            if (isInsideNamespace)
                return i;
        }

        // Last resort
        bool isPotentialBest = i < currentBest;
        bool containsCandidate = name.find(candidate) != std::string::npos;
        if (isPotentialBest and containsCandidate)
            return i;
    }

    return SIZE_T_MAX;
}

std::string parser_n::Listings::search(xml_node& node, const aliases_t& aliases) {
    std::string value;
    size_t bestRelevancyIndex = SIZE_T_MAX;
    for (xml_node child = node.first_child(); child; child = child.next_sibling()) {
        std::string name = child.name();

        const auto& index = relevancy(name, aliases, bestRelevancyIndex);
        if (index == SIZE_T_MAX or index >= bestRelevancyIndex)
            continue;
        value = child.first_child().value();
        bestRelevancyIndex = index;
    }

    return std::move(value);
}