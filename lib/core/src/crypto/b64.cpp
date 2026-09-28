#include <core/crypto/b64.hpp>


std::string b64Encode(unsigned char* const bytes, size_t size, int variant) {
    size_t b64Length = sodium_base64_ENCODED_LEN(size, variant);

    std::string token(b64Length, '\0');
    sodium_bin2base64(token.data(), b64Length,
                      bytes, size,
                      variant);

    // sodium_base64_ENCODED_LEN should also count the null terminator so we remove that extra size cause std::string don't need it.
    token.resize(b64Length - 1);

    return token;
}