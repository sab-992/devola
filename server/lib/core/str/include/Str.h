#pragma once

#include <iostream>

// TODO: Change from a function to the overload of the operator <<.
class StringFormattable_i {
public:
    virtual ~StringFormattable_i() = default;

    friend std::ostream& operator<<(std::ostream& Os, const StringFormattable_i& Object) {
        return Os << Object.ToString();
    }

protected:
    virtual std::string ToString() const = 0;
};