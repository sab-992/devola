#pragma once

#include <core/exception/logic.hpp>


class NotSupported : public LogicException {
public:
    NotSupported(const std::string& functionName);
    ~NotSupported() = default;
};