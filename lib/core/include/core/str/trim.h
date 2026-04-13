#pragma once

#include <core/conversion/converter.h>
#include <iostream>
#include <string>


template<typename T>
std::string lTrim(const T& element) {
    std::string string = Converter<T>::toString(element);

    if (string.empty())
        return "";

    size_t firstIndexNotWhiteSpace = string.find_first_not_of(" \t\n\r");

    if (firstIndexNotWhiteSpace == std::string::npos)
        return "";

        string.erase(0, firstIndexNotWhiteSpace);
    return string;
}

template<typename T>
std::string rTrim(const T& element) {
    std::string string = Converter<T>::toString(element);

    if (string.empty())
        return "";

    size_t lastIndexNotWhiteSpace = string.find_last_not_of(" \t\n\r");

    if (lastIndexNotWhiteSpace == std::string::npos)
        return "";

        string.erase(lastIndexNotWhiteSpace + 1);
    return string;
}

template<typename T>
std::string trim(const T& element) {
    return rTrim(lTrim(element));
}

