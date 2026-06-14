#include <core/exception/not_supported.h>


NotSupported::NotSupported(const std::string& functionName)
    : Exception(functionName, "Not supported", network_n::Code::NOT_ALLOWED) {}