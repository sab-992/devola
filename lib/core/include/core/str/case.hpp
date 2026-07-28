#pragma once

#include <functional>
#include <string>


namespace
{
    std::string changeCase(std::function<int(int)> caseFunction, std::string_view string) {
        std::string result;

        for (char character : string)
            result += caseFunction(character);

        return result;
    }
};

inline std::string toUpper(std::string_view string) {
    return changeCase([](char c){ return std::toupper(c); }, string);
}

inline std::string toLower(std::string_view string) {
    return changeCase([](char c){ return std::tolower(c); }, string);
}