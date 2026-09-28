#include <core/str/replace.hpp>


std::string replace(std::string_view element, std::string_view replacedToken, std::string_view replacementToken, size_t start, size_t end) {
    if (element.empty())
        return "";

    size_t indexReplacedToken = 0;
    size_t fixedEnd = end == std::string::npos ? element.size() : end;
    std::string string = std::string(element);
    while((indexReplacedToken = string.find(replacedToken, indexReplacedToken)) != std::string::npos and indexReplacedToken < fixedEnd)
        string.replace(indexReplacedToken, replacedToken.size(), replacementToken);

    return string;
}