#pragma once

#include <core/exception/exception.hpp>


class InvalidToken : public Exception {
public:
    InvalidToken(const std::string& functionName);
    ~InvalidToken() = default;
};