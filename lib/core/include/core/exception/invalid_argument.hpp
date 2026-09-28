#pragma once

#include <core/exception/logic.hpp>


class InvalidArgument : public LogicException {
public:
    InvalidArgument(const std::string& message, const std::string& argument="");
    ~InvalidArgument() = default;
};