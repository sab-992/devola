#include <core/str/hex.hpp>


std::string toHex(unsigned long num, bool uppercase) {
    std::stringstream ss;
    ss << std::hex << num;
    return uppercase ? toUpper(ss.str()) : toLower(ss.str());
}

unsigned long fromHex(std::string_view num) {
    return std::stoul(std::string(num), nullptr, 16);
}