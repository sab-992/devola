#pragma once

#include <string>
    
class StringFormattable_i {
public:
    virtual ~StringFormattable_i() = default;

    virtual std::string ToString() const = 0;
};