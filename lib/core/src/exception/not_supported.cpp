#include <core/exception/not_supported.hpp>


NotSupported::NotSupported(const std::string& functionName)
    : Exception(functionName, "Not supported", network_n::Code::NOT_ALLOWED) {}