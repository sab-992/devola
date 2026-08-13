#pragma once

#define JWT_DISABLE_PICOJSON

#include <chrono>
#include <core/utility/env.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <nlohmann/json.hpp>
#include <string>


class JWT {
    using json = nlohmann::json;

public:
    static std::string generate(const json& extra_claims);
    static std::string getToken(const http_n::Request& request);
    static void setToken(http_n::Response& response, std::string_view token);
    static json verify(const std::string& token);

private:
    inline static const std::string TOKEN_COOKIE_NAME = "jwt";
    inline static const std::string ISSUER = "Devola";
    inline static const std::chrono::seconds TTL = std::chrono::hours(24);

    static std::string secret();
};
