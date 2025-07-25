#pragma once

#include <iostream>
#include <string>

template<typename T>
std::string LTrim(T Element) {
    if(not std::is_convertible_v<T, std::string>)
        return "";

    std::string String = static_cast<std::string>(Element);
    if (String.empty())
        return "";

    size_t FirstIndexNotWhiteSpace = String.find_first_not_of(' ');

    if (FirstIndexNotWhiteSpace == std::string::npos)
        return "";

        String.erase(0, FirstIndexNotWhiteSpace);
    return String;
}

template<typename T>
std::string RTrim(T Element) {
    if(not std::is_convertible_v<T, std::string>)
        return "";

    std::string String = static_cast<std::string>(Element);
    if (String.empty())
        return "";

    size_t LastIndexNotWhiteSpace = String.find_last_not_of(" \t\n\r");

    if (LastIndexNotWhiteSpace == std::string::npos)
        return "";

        String.erase(LastIndexNotWhiteSpace + 1);
    return String;
}

template<typename T>
std::string Trim(T Element) {
    if(not std::is_convertible_v<T, std::string>)
        return "";

    return RTrim(LTrim(Element));
}

