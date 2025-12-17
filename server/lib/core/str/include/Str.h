#pragma once

#include <string>
    
class StringConvertible_i {
public:
    virtual ~StringConvertible_i() = default;

    virtual std::string ToString() const = 0;
};