#include <core/exception/logic.hpp>


LogicException::LogicException(const std::string& message)
    : Exception(message, "Logic exception", network_n::Code::SERVER_ERROR) {}