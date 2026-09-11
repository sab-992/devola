#pragma once

#include <sodium.h>
#include <string>


class Cryptography;

class HashAlgorithm_i {
    friend class Cryptography;

public:
    virtual ~HashAlgorithm_i() = default;

protected:
    virtual std::string hash(std::string_view secret) const = 0;
    virtual void verify(std::string_view storedHash, std::string_view secret) const = 0;
};