#include <core/str/case.hpp>


namespace {
    std::string changeCase(std::function<int(int)> caseFunction, std::string_view string) {
        std::string result;

        for (char character : string)
            result += caseFunction(character);

        return result;
    }
}

std::string toUpper(std::string_view string) {
    return changeCase([](char c){ return std::toupper(c); }, string);
}

std::string toLower(std::string_view string) {
    return changeCase([](char c){ return std::tolower(c); }, string);
}