#pragma once

#include <iostream>


class StringConvertible {
public:
    virtual ~StringConvertible() = default;

    friend std::ostream& operator<<(std::ostream& os, const StringConvertible& object) {
        return os << object.toString();
    }

    virtual std::string toString() const = 0;
};