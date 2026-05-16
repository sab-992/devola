#pragma once

#include <core/conversion/converter.h>
#include <string>
#include <vector>


// Splits the string at the given range [start, end) using the specified delimiter and stores the results in the provided vector.
inline void split(std::string_view element, std::vector<std::string>& resultVector, std::string_view splittingToken=" ", size_t start=0, size_t end=std::string::npos) {
    size_t countFromStart = end == std::string::npos ? end : end - start;
    std::string_view string = element.substr(start, countFromStart);

    size_t indexSplitToken;
    while((indexSplitToken = string.find(splittingToken)) != std::string::npos) {
        const std::string_view part = string.substr(0, indexSplitToken);
        resultVector.push_back(std::string(part));
        string = string.substr(indexSplitToken + splittingToken.size());
    }
    resultVector.push_back(std::string(string));
}