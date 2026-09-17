#include <core/jwt/jwt.hpp>


void JWT::clearBrowserToken(http_n::Response& response) {
    response.setHeader("Set-Cookie", std::format("{}=; HttpOnly; Secure; Max-Age=0; SameSite=Strict; Path=/", TOKEN_COOKIE_NAME));
}

void JWT::generateJWT(const json& extra_claims, http_n::Response& response) {
    if (not extra_claims.is_object())
        throw InvalidArgument("must be a JSON object", "Extra claims");

    const auto now = Time::now();
    auto token_builder = jwt::create().set_type("JWT")
                                      .set_issuer(ISSUER)
                                      .set_issued_at(now)
                                      .set_expires_at(now + TTL);

    for (auto it = extra_claims.begin(); it != extra_claims.end(); ++it)
        token_builder.set_payload_claim(it.key(), jwt::claim(it.value()));

    response.setCookie(TOKEN_COOKIE_NAME, Cookie().setName(TOKEN_COOKIE_NAME)
                                                  .setValue(token_builder.sign(jwt::algorithm::hs256{secret()}))
                                                  .setRestrictionToBrowser(true)
                                                  .setMaxAge(TTL)
                                                  .setPath("/").build());
}

std::string JWT::generateRefreshToken(http_n::Response& response) {
    unsigned char buf[TOKEN_BYTES];
    randombytes_buf(buf, sizeof(buf));
    const std::string& refreshToken = b64Encode(buf, sizeof(buf), sodium_base64_VARIANT_URLSAFE_NO_PADDING);
    sodium_memzero(buf, sizeof(buf));
    response.setCookie(REFRESH_TOKEN_COOKIE_NAME, Cookie().setName(REFRESH_TOKEN_COOKIE_NAME)
                                                          .setValue(refreshToken)
                                                          .setRestrictionToBrowser(true)
                                                          .setMaxAge(REFRESH_TTL)
                                                          .setPath(REFRESH_TOKEN_ENDPOINT).build());
    return m_crypto->hash(Generic::instance(), refreshToken);
}

std::string JWT::getToken(const http_n::Request& request) {
    try {
        return request.cookie(TOKEN_COOKIE_NAME).value();
    } catch (const std::exception& e) {
        return "";
    }
}

std::string JWT::getRefreshToken(const http_n::Request& request) {
    try {
        return request.cookie(REFRESH_TOKEN_COOKIE_NAME).value();
    } catch (const std::exception& e) {
        return "";
    }
}

std::string JWT::secret() {
    return env(std::format("{}/settings/.env", ROOT_DIRECTORY))["JWT_SECRET"];
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