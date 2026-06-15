#pragma once

#include <core/exception/exception.hpp>


class InvalidArgument : public Exception {
public:
    InvalidArgument(const std::string& message, const std::string& argument="");
    ~InvalidArgument() = default;
};