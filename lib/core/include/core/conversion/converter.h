#pragma once

#include <sstream>
#include <string>


template <typename T>
class Converter {
public:
    static std::string toString(const T& param) {
        try {
            std::ostringstream oss;
            oss << param;
            return oss.str();
        }
        catch(const std::exception& e) {
            return "";
        }
    }
};