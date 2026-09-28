#include <core/str/split.hpp>


void split(std::string_view element, std::vector<std::string>& resultVector, std::string_view splittingToken, size_t start, size_t end) {
    if (element.empty()) {
        resultVector.emplace_back("");
        return;
    }

    size_t countFromStart = end == std::string::npos ? end : end - start;
    std::string_view string = element.substr(start, countFromStart);

    size_t indexSplitToken;
    while((indexSplitToken = string.find(splittingToken)) != std::string::npos) {
        std::string_view part = string.substr(0, indexSplitToken);
        resultVector.emplace_back(part);
        string = string.substr(indexSplitToken + splittingToken.size());
    }

    resultVector.emplace_back(string);
}

std::vector<std::string> split(std::string_view element, std::string_view splittingToken, size_t start, size_t end) {
    std::vector<std::string> result;
    split(element, result, splittingToken, start, end);
    return result;
}