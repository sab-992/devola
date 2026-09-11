#include <core/crypto/detail/hash/generic.hpp>


Generic::Generic(const Private_s&) {
    m_light = log_n::Light::instance();
}

void Generic::verify(std::string_view storedHash, std::string_view secret) const {
    m_light->log(log_n::Level_en::WARNING, "Empty function", std::format("\"{}\"", FUNCTION_SIGNATURE), "called for Generic hash algorithm.");
}

std::string Generic::hash(std::string_view secret) const {
    unsigned char hashedSecret[crypto_generichash_BYTES];
    crypto_generichash(hashedSecret, sizeof(hashedSecret),
                       reinterpret_cast<const unsigned char*>(secret.data()), secret.size(),
                       nullptr, 0);

    const int variant = sodium_base64_VARIANT_ORIGINAL_NO_PADDING;
    size_t base64Length = sodium_base64_ENCODED_LEN(sizeof(hashedSecret), variant);

    std::vector<char> b64(base64Length);
    sodium_bin2base64(b64.data(), base64Length, hashedSecret, sizeof(hashedSecret), variant);

    return std::string(b64.data());
}