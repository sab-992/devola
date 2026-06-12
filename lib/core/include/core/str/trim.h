#pragma once

#include <string>
#include <string_view>
#include <type_traits>


template <typename T>
    requires std::same_as<T, std::string> || std::is_convertible_v<T, std::string_view>
using StringResult = std::conditional_t<std::is_same_v<T, std::string_view>, std::string_view, std::string>;

template <typename T>
inline StringResult<T> lTrim(const T& param) {
    StringResult<T> string(param);
    if (string.empty())
        return string;

    size_t firstIndexNotWhiteSpace = string.find_first_not_of(" \t\n\r");

    if (firstIndexNotWhiteSpace == std::string::npos)
        return "";

    return string.substr(firstIndexNotWhiteSpace);
}

template <typename T>
inline StringResult<T> rTrim(const T& param) {
    StringResult<T> string(param);

    if (string.empty())
        return string;

    size_t lastIndexNotWhiteSpace = string.find_last_not_of(" \t\n\r");

    if (lastIndexNotWhiteSpace == std::string::npos)
        return "";

    return string.substr(0, lastIndexNotWhiteSpace + 1);
}

template <typename T>
inline StringResult<T> trim(const T& param) { return rTrim(lTrim(param)); }