#pragma once

#include <exception>
#include <string>


class Exception : public std::exception {
public:
    Exception(const std::string& message);
    ~Exception();

    const char* what() const throw() override;
    constexpr const char* type() const throw() { return m_type; };

protected:
    const std::string m_message;
    const char* m_type;

    Exception(const std::string& message, const char* type);
};