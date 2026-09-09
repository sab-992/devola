#include <core/exception/invalid_token.hpp>


InvalidToken::InvalidToken(const std::string& message)
    : LogicException(message, "Invalid token", Code::UNAUTHORIZED) {}