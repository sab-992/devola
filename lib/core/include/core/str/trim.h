#pragma once

#include <iostream>
#include <string>


template<typename T>
std::string lTrim(T element) {
    if(not std::is_convertible_v<T, std::string>)
        return "";

    std::string string = static_cast<std::string>(element);
    if (string.empty())
        return "";

    size_t firstIndexNotWhiteSpace = string.find_first_not_of(" \t\n\r");

    if (firstIndexNotWhiteSpace == std::string::npos)
        return "";

        string.erase(0, firstIndexNotWhiteSpace);
    return string;
}

template<typename T>
std::string rTrim(T element) {
    if(not std::is_convertible_v<T, std::string>)
        return "";

    std::string string = static_cast<std::string>(element);
    if (string.empty())
        return "";

    size_t lastIndexNotWhiteSpace = string.find_last_not_of(" \t\n\r");

    if (lastIndexNotWhiteSpace == std::string::npos)
        return "";

        string.erase(lastIndexNotWhiteSpace + 1);
    return string;
}

template<typename T>
std::string trim(T element) {
    if(not std::is_convertible_v<T, std::string>)
        return "";

    return rTrim(lTrim(element));
}

