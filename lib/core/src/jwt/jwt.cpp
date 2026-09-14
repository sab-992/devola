#include <core/jwt/jwt.hpp>


void JWT::clearBrowserToken(http_n::Response& response) {
    response.setHeader("Set-Cookie", std::format("{}=; HttpOnly; Secure; Max-Age=0; SameSite=Strict; Path=/", TOKEN_COOKIE_NAME));
}

std::string JWT::generate(const json& extra_claims, http_n::Response& response) {
    const std::string& jwt = generateJWT(extra_claims);
    const std::string& refreshToken = generateRefreshToken();

    setToken(response, jwt, TOKEN_COOKIE_NAME, "/");
    setToken(response, refreshToken, REFRESH_TOKEN_COOKIE_NAME, REFRESH_TOKEN_ENDPOINT);

    return m_crypto->hash(Generic::instance(), refreshToken);
}

std::string JWT::generateJWT(const json& extra_claims) {
    if (not extra_claims.is_object())
        throw InvalidArgument("must be a JSON object", "Extra claims");

    const auto now = std::chrono::system_clock::now();
    auto token_builder = jwt::create().set_type("JWT")
                                    .set_issuer(ISSUER)
                                    .set_issued_at(now)
                                    .set_expires_at(now + TTL);

    for (auto it = extra_claims.begin(); it != extra_claims.end(); ++it)
        token_builder.set_payload_claim(it.key(), jwt::claim(it.value()));

    return token_builder.sign(jwt::algorithm::hs256{secret()});
}

std::string JWT::generateRefreshToken() {
    unsigned char buf[TOKEN_BYTES];
    randombytes_buf(buf, sizeof(buf));
    const std::string& refreshToken = b64Encode(buf, sizeof(buf), sodium_base64_VARIANT_URLSAFE_NO_PADDING);
    sodium_memzero(buf, sizeof(buf));
    return refreshToken;
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

void JWT::setToken(http_n::Response& response, std::string_view token, const std::string& cookieName, std::string_view path) {
    response.setCookie(cookieName, Cookie().setName(cookieName)
                                           .setValue(token)
                                           .setRestrictionToBrowser(true)
                                           .setMaxAge(TTL)
                                           .setPath(path).build());
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