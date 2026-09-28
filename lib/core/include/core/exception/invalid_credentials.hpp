#pragma once

#include <core/exception/logic.hpp>


class InvalidCredentials : public LogicException {
public:
    InvalidCredentials(const std::string& message);
    ~InvalidCredentials() = default;
};