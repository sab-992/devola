#pragma once

#include <core/exception/exception.hpp>


class NotSupported : public Exception {
public:
    NotSupported(const std::string& functionName);
    ~NotSupported() = default;
};