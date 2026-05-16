#include <core/exception/invalid_argument.h>


InvalidArgument::InvalidArgument(const std::string& message, const std::string& argument)
    : Exception(message, "Invalid argument", network_n::Code::BAD_REQUEST) {
    if (not argument.empty())
        this->m_message = std::format("{} - {}", argument, this->m_message);
}