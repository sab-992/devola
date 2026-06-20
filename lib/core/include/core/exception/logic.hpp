#pragma once

#include <core/exception/exception.hpp>


class LogicException : public Exception {
public:
    LogicException(const std::string& message);
    ~LogicException() = default;
};