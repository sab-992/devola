#pragma once

#include <core/str/case.h>
#include <sstream>
#include <string>


inline std::string toHex(unsigned long num, bool uppercase=true) {
    std::stringstream ss;
    ss << std::hex << num;
    return uppercase ? toUpper(ss.str()) : toLower(ss.str());
}

inline unsigned long fromHex(std::string_view num) {
    return std::stoul(std::string(num), nullptr, 16);
}