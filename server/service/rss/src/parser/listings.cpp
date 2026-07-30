#include <parser/listings.hpp>


void parser_n::Listings::normalize(record_t& record) {
    if (record.empty() or recordContains(record, "company"))
        return;

    const std::string& title = record["title"].asString();
    record["company"] = title.substr(0, title.find(":"));
}

std::tuple<int, parser_n::Listings::websiteInfo_t, std::vector<database_n::record_t>> parser_n::Listings::parse(const xml_n::Document& document) {
    xml_node channelNode = document.find_node([](const xml_node& node) -> bool { return strcmp(node.name(), "channel") == 0; });

    if (not channelNode)
        throw InvalidArgument("Invalid XML format", "XML document");

    unsigned int ttl = UINT_MAX;
    websiteInfo_t websiteInformation;
    std::vector<database_n::record_t> records;

    for (xml_node node = channelNode.first_child(); node; node = node.next_sibling()) {
        std::string name = node.name();

        if (TTL_ALIASES.contains(name))
            ttl = parseTTL(node);
        else if (name == "item") {
            record_t record = addRelevantNodes<database_n::Value>(node, ITEM_NODES_ALIASES);
            if (not record.empty())
                records.emplace_back(std::move(record));
        }
    }

    return { std::move(ttl), std::move(websiteInformation), std::move(records) };
}

int parser_n::Listings::parseTTL(const xml_node& ttl) {
    return std::stoi(ttl.first_child().value());
}

bool parser_n::Listings::recordContains(const record_t& record, const std::string& key) {
    return record.contains(key) and not trim(record.at("company").asString()).empty();
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