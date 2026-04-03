#include <core/exception/not_supported.h>

NotSupported::NotSupported(const std::string& message) : Exception(message, "Not supported") {}
NotSupported::~NotSupported() {}