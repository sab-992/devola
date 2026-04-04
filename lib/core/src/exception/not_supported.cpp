#include <core/exception/not_supported.h>


NotSupported::NotSupported(const std::string& message)
    : Exception(message, "Not supported", network_n::Code::NOT_ALLOWED) {}

NotSupported::~NotSupported() {}