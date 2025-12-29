#pragma once

#include <sstream>
#include <string>

template <typename T>
class Converter {
public:
    static std::string ToString(const T& Param) {
        try {
            std::ostringstream Oss;
            Oss << Param;
            return Oss.str();
        }
        catch(const std::exception& e) {
            return "";
        }
    }
};