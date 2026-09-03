#include <core/exception/invalid_token.hpp>


InvalidToken::InvalidToken(const std::string& functionName)
    : Exception(functionName, "Invalid token", network_n::Code::UNAUTHORIZED) {}