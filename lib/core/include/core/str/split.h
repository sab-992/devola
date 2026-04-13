#pragma once

#include <core/conversion/converter.h>
#include <string>
#include <vector>


// Splits the string at the given range [start, end) using the specified delimiter and stores the results in the provided vector.
template<typename T>
void split(const T& element, std::vector<std::string>& resultVector, const std::string& splittingToken=" ", size_t start=0, size_t end=std::string::npos) {
    size_t countFromStart = end == std::string::npos ? end : end - start;
    std::string string = Converter<T>::toString(element).substr(start, countFromStart);

    size_t indexSplitToken;
    while((indexSplitToken = string.find(splittingToken)) != std::string::npos) {
        const std::string part = string.substr(0, indexSplitToken);
        resultVector.push_back(part);
        string = string.substr(indexSplitToken + splittingToken.size());
    }
    resultVector.push_back(string);
}