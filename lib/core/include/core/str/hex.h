#pragma once

#include <sstream>
#include <string>

inline std::string toHex(unsigned long num) {
    using namespace std;

    stringstream ss;
    ss << std::hex << num;
    return ss.str();
}

inline unsigned long fromHex(const std::string& num) {
    return std::stoul(num, nullptr, 16);
}