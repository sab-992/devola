#pragma once

#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using HeadersUMap_t = std::unordered_map<std::string, std::string>;
using json = nlohmann::json;

namespace Http_n {
    struct Endpoint { std::string Host; int Port; };

    struct Status { 
        int Code; 
        std::string Reason; 
        std::string ToString() { return std::format("{} {}", Code, Reason); }; 
    };
}