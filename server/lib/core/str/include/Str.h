#pragma once

#include <string>

// TODO: Change from a function to the overload of the operator <<.
class StringFormattable_i {
public:
    virtual ~StringFormattable_i() = default;

    virtual std::string ToString() const = 0;
};