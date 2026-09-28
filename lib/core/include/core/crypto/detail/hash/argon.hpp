#pragma once

#include <core/crypto/interface/hash_algorithm.hpp>
#include <core/exception/invalid_credentials.hpp>
#include <core/utility/singleton.hpp>


class Argon2id : public HashAlgorithm_i,  public Singleton<Argon2id> {
public:
    Argon2id(const Private_s&);
    ~Argon2id() = default;

protected:
    std::string hash(std::string_view secret) const override;
    void verify(std::string_view storedHash, std::string_view secret) const override;
};