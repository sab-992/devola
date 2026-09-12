#pragma once

#define JWT_DISABLE_PICOJSON

#include <chrono>
#include <core/utility/env.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <core/network/cookie.hpp>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <nlohmann/json.hpp>
#include <string>


class JWT {
    using json = nlohmann::json;
    using Cookie = network_n::Cookie;

public:
    static void clearBrowserToken(http_n::Response& response);
    static void generate(const json& extra_claims, http_n::Response& response);

    static std::string getToken(const http_n::Request& request);

    static json verify(std::string_view token);
    static json verify(const http_n::Request& request);

private:
    inline static const std::string TOKEN_COOKIE_NAME = "jwt";
    inline static const std::string ISSUER = "Devola";
    inline static const std::chrono::seconds TTL = std::chrono::minutes(15);

    static void setToken(http_n::Response& response, std::string_view token);

    static std::string secret();
};
