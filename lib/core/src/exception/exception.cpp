#include <core/exception/exception.h>

Exception::Exception(const std::string& what) : m_what(what) {}
Exception::~Exception() {}

const char* Exception::what() const throw() {
    return m_what.c_str();
}

constexpr const char* Exception::type() const throw() {
    return "Exception";
}