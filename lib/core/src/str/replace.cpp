#include <core/str/replace.hpp>


std::string replace(std::string_view element, std::string_view replacedToken, std::string_view replacementToken, size_t start, size_t end) {
    size_t countFromStart = end == std::string::npos ? end : end - start;
    std::string string = std::string(element.substr(start, countFromStart));

    size_t indexReplacedToken;
    while((indexReplacedToken = string.find(replacedToken)) != std::string::npos)
        string.replace(indexReplacedToken, indexReplacedToken + replacedToken.size(), replacementToken);

    return string;
}