#pragma once

#include <core/exception/exception.hpp>
#include <core/crypto/detail/hash.hpp>
#include <core/str/trim.hpp>
#include <core/utility/singleton.hpp>
#include <sodium.h>
#include <string>


class HashAlgorithm_i;

class Cryptography : public Singleton<Cryptography> {
    class HashAlgorithmFinder {
    public:
        HashAlgorithmFinder() = delete;
        ~HashAlgorithmFinder() = default;

        static std::shared_ptr<HashAlgorithm_i> get(std::string_view hashedSecret);

    private:
        inline static const std::unordered_map<std::string, std::shared_ptr<HashAlgorithm_i>> m_algorithms = { { "argon2id", Argon2id::instance() },
                                                                                                               { "generic",  Generic::instance() } };
    };

public:
    Cryptography(const Private_s&);
    ~Cryptography() = default;

    std::string hash(std::shared_ptr<HashAlgorithm_i> algorithm, std::string_view secret) const;
    void verifyHash(std::string_view storedHash, std::string_view secret) const;
};