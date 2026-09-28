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

    return b64Encode(hashedSecret, sizeof(hashedSecret), sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
}