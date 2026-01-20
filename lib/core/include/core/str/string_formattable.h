#pragma once

#include <iostream>


// TODO: Change from a function to the overload of the operator <<.
class StringFormattable_i {
public:
    virtual ~StringFormattable_i() = default;

    friend std::ostream& operator<<(std::ostream& os, const StringFormattable_i& object) {
        return os << object.toString();
    }

protected:
    virtual std::string toString() const = 0;
};