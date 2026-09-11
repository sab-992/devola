#include <core/exception/invalid_credentials.hpp>


InvalidCredentials::InvalidCredentials(const std::string& message)
    : LogicException(message, "Invalid credentials", Code::UNAUTHORIZED) {}