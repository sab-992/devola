#pragma once

#include <core/network/network.h>
#include <exception>
#include <string>


class Exception : public std::exception {
public:
    Exception(const std::string& message);
    ~Exception();

    const char* what() const noexcept override;
    constexpr const char* type() const noexcept { return m_type; };
    constexpr network_n::Code code() const noexcept { return m_httpCodeEquivalent; };

protected:
    const std::string m_message;
    const char* m_type;
    const network_n::Code m_httpCodeEquivalent;

    Exception(const std::string& message, const char* type, const network_n::Code& httpCodeEquivalent);
};