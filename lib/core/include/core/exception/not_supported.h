#pragma once

#include <core/exception/exception.h>


class NotSupported : public Exception {
public:
    NotSupported(const std::string& message);
    ~NotSupported();
};