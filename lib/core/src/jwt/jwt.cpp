#include <core/jwt/jwt.hpp>


void JWT::clearBrowserToken(http_n::Response& response) {
    response.setHeader("Set-Cookie", std::format("{}=; HttpOnly; Secure; Max-Age=0; SameSite=Strict; Path=/", TOKEN_COOKIE_NAME));
}

void JWT::generate(const json& extra_claims, http_n::Response& response) {
    if (not extra_claims.is_object())
        throw InvalidArgument("must be a JSON object", "Extra claims");

    const auto now = std::chrono::system_clock::now();
    auto token_builder = jwt::create().set_type("JWT")
                                      .set_issuer(ISSUER)
                                      .set_issued_at(now)
                                      .set_expires_at(now + TTL);

    for (auto it = extra_claims.begin(); it != extra_claims.end(); ++it)
        token_builder.set_payload_claim(it.key(), jwt::claim(it.value()));

    setToken(response, token_builder.sign(jwt::algorithm::hs256{secret()}));
}

std::string JWT::getToken(const http_n::Request& request) {
    try {
        return request.cookie(TOKEN_COOKIE_NAME).value();
    } catch (const std::exception& e) {
        return "";
    }
}

std::string JWT::secret() {
    return env(std::format("{}/settings/.env", ROOT_DIRECTORY))["JWT_SECRET"];
}

void JWT::setToken(http_n::Response& response, std::string_view token) {
    response.setCookie(TOKEN_COOKIE_NAME, Cookie().setName(TOKEN_COOKIE_NAME)
                                                  .setValue(token)
                                                  .setRestrictionToBrowser(true)
                                                  .setMaxAge(TTL)
                                                  .setPath("/").build());
}

nlohmann::json JWT::verify(std::string_view token) {
    try {
        if (token.empty())
            throw InvalidToken("JSON web token is empty");

        auto decoded = jwt::decode(std::string(token));
        auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256{secret()})
                                     .with_issuer(ISSUER);

        verifier.verify(decoded);
        return json(decoded.get_payload_json());
    } catch (const std::exception& e) {
        throw InvalidToken(e.what());
    }
}

nlohmann::json JWT::verify(const http_n::Request& request) {
    return verify(getToken(request));
}