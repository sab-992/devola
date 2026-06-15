#include <core/exception/exception.hpp>


Exception::Exception(const std::string& message)
: Exception(message, "Exception", network_n::Code::SERVER_ERROR) {}

Exception::Exception(const std::string& message, const char* type, const network_n::Code& httpCodeEquivalent)
: m_message(message), m_type(type), m_httpCodeEquivalent(httpCodeEquivalent) {}

const char* Exception::what() const noexcept { return m_message.c_str(); }

std::string Exception::toString() const {
    std::ostringstream stringBuilder;
    stringBuilder << m_type << ": " << m_message;
    return stringBuilder.str();
}