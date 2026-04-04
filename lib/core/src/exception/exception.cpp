#include <core/exception/exception.h>

Exception::Exception(const std::string& message) 
    : Exception(message, "Exception", network_n::Code::SERVER_ERROR) {}

Exception::Exception(const std::string& message, const char* type, const network_n::Code& httpCodeEquivalent) 
    : m_message(message), m_type(type), m_httpCodeEquivalent(httpCodeEquivalent) {}

Exception::~Exception() {}

const char* Exception::what() const noexcept { return m_message.c_str(); }