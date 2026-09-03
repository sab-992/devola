#include <core/exception/database.hpp>


DatabaseException::DatabaseException(const std::string& functionName)
    : Exception(functionName, "Database exception", network_n::Code::SERVER_ERROR) {}