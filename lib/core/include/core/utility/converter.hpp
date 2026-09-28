#pragma once

#include <sstream>
#include <string>


class Converter {
public:
    static std::string toString(const auto& param) {
        try {
            if constexpr (std::is_same_v<std::decay_t<decltype(param)>, std::string>)
                return param;
            else if constexpr (std::is_convertible_v<std::decay_t<decltype(param)>, std::string>)
                return std::string(param);
            else if constexpr (std::is_same_v<std::decay_t<decltype(param)>, bool>)
                return param ? "true" : "false";

            std::ostringstream oss;
            oss << param;
            return oss.str();
        }
        catch(const std::exception& e) {
            return "";
        }
    }
};