#pragma once

#include <sstream>
#include <string>

std::string toHex(unsigned long num) {
    using namespace std;

    stringstream ss;
    ss << std::hex << num;
    return ss.str();
}

unsigned long fromHex(const std::string& num) {
    return std::stoul(num, nullptr, 16);
}