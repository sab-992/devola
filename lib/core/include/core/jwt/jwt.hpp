#pragma once

#define JWT_DISABLE_PICOJSON

#include <chrono>
#include <core/crypto/b64.hpp>
#include <core/crypto/crypto.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <core/network/cookie.hpp>
#include <core/utility/env.hpp>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <nlohmann/json.hpp>
#include <string>


class JWT {
    using json = nlohmann::json;
    using Cookie = network_n::Cookie;

    inline static const std::string ISSUER = "Devola";
    inline static const std::string REFRESH_TOKEN_COOKIE_NAME = "rjwt";
    inline static const std::string REFRESH_TOKEN_ENDPOINT = "/user/auth";
    inline static const int TOKEN_BYTES = 32;
    inline static const std::string TOKEN_COOKIE_NAME = "jwt";

public:
    inline static const std::chrono::seconds TTL = std::chrono::minutes(15);
    inline static const std::chrono::seconds REFRESH_TTL = std::chrono::days(7);

    static void clearBrowserToken(http_n::Response& response);
    static void generateJWT(const json& extra_claims, http_n::Response& response);
    static std::string generateRefreshToken(http_n::Response& response);

    static std::string getToken(const http_n::Request& request);
    static std::string getRefreshToken(const http_n::Request& request);

    static json verify(std::string_view token);
    static json verify(const http_n::Request& request);

private:
    inline static std::shared_ptr<Cryptography> m_crypto = Cryptography::instance();

    static std::string secret();
};
