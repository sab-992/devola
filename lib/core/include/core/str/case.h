#pragma once

#include <functional>
#include <string>


inline namespace {
    std::string changeCase(std::function<int(int)> caseFunction, const std::string& string) {
        std::string result;

        for (char character : string)
            result += caseFunction(character);

        return result;
    }
};

inline std::string toUpper(const std::string& string) {
    return changeCase([](char c){ return std::toupper(c); }, string);
}

inline std::string toLower(const std::string& string) {
    return changeCase([](char c){ return std::tolower(c); }, string);
}