#pragma once

#include <exception>
#include <string>


class Exception : std::exception {
public:
    Exception(const std::string& what) : m_what(what) {}

    const char* what() const throw() override { return m_what.c_str(); }
    constexpr const char* type() const throw() { return "Exception"; }

private:
    const std::string m_what;
};