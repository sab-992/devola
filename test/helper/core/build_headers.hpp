#pragma once

#include <format>
#include <unordered_map>
#include <string>


inline std::string buildHeaders(const std::unordered_map<std::string, std::string>& headersUMap) {
    std::string headers;

    for (const auto& [header, value] : headersUMap)
        headers += std::format("{}: {}\r\n", header, value);

    return headers.substr(0, headers.size() - 2);
}