#include <core/jwt/jwt.hpp>


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
    const std::string& cookie = request.header("Cookie");
    size_t pos = cookie.find(TOKEN_COOKIE_NAME + "=");

    if (pos == std::string::npos)
        return "";

    pos += TOKEN_COOKIE_NAME.length() + 1;
    size_t end = cookie.find(";", pos);
    if (end == std::string::npos)
        end = cookie.length();

    return cookie.substr(pos, end - pos);
}

std::string JWT::secret() {
    return env(std::format("{}/settings/.env", ROOT_DIRECTORY))["JWT_SECRET"];
}

void JWT::setToken(http_n::Response& response, std::string_view token) {
    response.setHeader("Set-Cookie", std::format("{}={}; HttpOnly; Secure; SameSite=Strict; Path=/", TOKEN_COOKIE_NAME, token));
}

nlohmann::json JWT::verify(std::string_view token) {
    try {
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