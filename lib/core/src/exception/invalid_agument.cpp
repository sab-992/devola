#include <core/exception/invalid_argument.hpp>


InvalidArgument::InvalidArgument(const std::string& message, const std::string& argument)
    : LogicException(message, "Invalid argument", Code::BAD_REQUEST) {
    if (not argument.empty())
        this->m_message = std::format("{} - {}", argument, message);
}