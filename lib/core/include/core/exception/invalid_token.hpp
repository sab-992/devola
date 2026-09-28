#pragma once

#include <core/exception/logic.hpp>


class InvalidToken : public LogicException {
public:
    InvalidToken(const std::string& message);
    ~InvalidToken() = default;
};