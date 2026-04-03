#include <core/exception/exception.h>

Exception::Exception(const std::string& message) { Exception::Exception(message, "Exception"); }

Exception::Exception(const std::string& message, const char* type) : m_message(message), m_type(type) {}

Exception::~Exception() {}

const char* Exception::what() const noexcept { return m_message.c_str(); }