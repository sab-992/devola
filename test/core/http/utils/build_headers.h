#pragma once

#include <format>
#include <map>
#include <string>


inline std::string buildHeaders(const std::map<std::string, std::string>& headersMap) {
    std::string headers;

    for (const auto& [header, value] : headersMap)
        headers += std::format("{}: {}\r\n", header, value);

    return headers;
}