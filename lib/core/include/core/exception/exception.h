#pragma once

#include <exception>
#include <string>


class Exception : std::exception {
public:
    Exception(const std::string& what);
    ~Exception();

    const char* what() const throw() override;
    constexpr const char* type() const throw();

private:
    const std::string m_what;
};