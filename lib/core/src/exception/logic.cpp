#include <core/exception/logic.hpp>


LogicException::LogicException(const std::string& message, const char* type, Code httpCodeEquivalent)
    : Exception(message, type, httpCodeEquivalent) {}