#include <core/exception/database.hpp>


DatabaseException::DatabaseException(const std::string& message)
    : Exception(message, "Database exception", Code::SERVER_ERROR) {}