#include <core/crypto/crypto.hpp>


std::shared_ptr<HashAlgorithm_i> Cryptography::HashAlgorithmFinder::get(std::string_view hashedSecret) {
    if(hashedSecret.at(0) != '$')
        return m_algorithms.at("generic");

    std::string algorithm = std::string(hashedSecret.substr(1, hashedSecret.find('$', 1) - 1));

    if (trim(algorithm).empty() or not m_algorithms.contains(algorithm))
        throw Exception(std::format("Couldn't determine the algorithm used for hashing. Given algorithm: \"{}\"", algorithm));

    return m_algorithms.at(algorithm);
}

Cryptography::Cryptography(const Private_s&) {
    if (sodium_init() < 0)
        throw Exception("Could not initialize libsodium");
}

std::string Cryptography::hash(std::shared_ptr<HashAlgorithm_i> algorithm, std::string_view secret) const {
    return algorithm->hash(secret);
}

void Cryptography::verifyHash(std::string_view storedHash, std::string_view secret) const {
    HashAlgorithmFinder::get(storedHash)->verify(storedHash, secret);
}