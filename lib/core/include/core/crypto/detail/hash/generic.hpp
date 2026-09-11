#pragma once

#include <core/crypto/interface/hash_algorithm.hpp>
#include <core/exception/invalid_credentials.hpp>
#include <core/logging/light.hpp>
#include <core/utility/singleton.hpp>
#include <core/utility/function.hpp>


class Generic : public HashAlgorithm_i,  public Singleton<Generic> {
public:
    Generic(const Private_s&);
    ~Generic() = default;

protected:
    std::string hash(std::string_view secret) const override;
    void verify(std::string_view storedHash, std::string_view secret) const override;

private:
    std::shared_ptr<log_n::Light> m_light;
};