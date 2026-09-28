#include <core/exception/not_supported.hpp>


NotSupported::NotSupported(const std::string& functionName)
    : LogicException(functionName, "Not supported", Code::NOT_ALLOWED) {}