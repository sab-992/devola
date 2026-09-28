#include <core/crypto/detail/hash/argon.hpp>


Argon2id::Argon2id(const Private_s&) {}

void Argon2id::verify(std::string_view storedHash, std::string_view secret) const {
    if (crypto_pwhash_str_verify(storedHash.data(), secret.data(), secret.size()) != 0)
        throw InvalidCredentials("Non equality in hash comparison");
}

std::string Argon2id::hash(std::string_view secret) const {
    char hashedSecret[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str_alg(hashedSecret,
                              secret.data(), secret.size(),
                              crypto_pwhash_OPSLIMIT_INTERACTIVE,
                              crypto_pwhash_MEMLIMIT_INTERACTIVE,
                              crypto_pwhash_ALG_ARGON2ID13) != 0) {
        throw Exception("Ran out of memory while hashing secrets");
    }

    return hashedSecret;
}