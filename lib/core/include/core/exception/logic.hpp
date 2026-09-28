#pragma once

#include <core/exception/exception.hpp>


class LogicException : public Exception {
public:
    LogicException(const std::string& message, const char* type="Logic exception", Code httpCodeEquivalent=Code::SERVER_ERROR);
    ~LogicException() = default;
};