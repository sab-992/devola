#pragma once

#include <core/exception/exception.hpp>


class DatabaseException : public Exception {
public:
    DatabaseException(const std::string& message);
    ~DatabaseException() = default;
};