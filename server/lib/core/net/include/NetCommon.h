#pragma once

#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using HeadersUMap_t = std::unordered_map<std::string, std::string>;
using json = nlohmann::json;

namespace Net_n {
    struct Endpoint {
        std::string Host;
        int Port;
        std::string ToString() { return std::format("{}:{}", Host, Port); };
    };

    struct Status { 
        int Code; 
        std::string Reason; 
        std::string ToString() { return std::format("{} {}", Code, Reason); }; 
    };
}