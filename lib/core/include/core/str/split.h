#pragma once

#include <core/conversion/converter.h>
#include <string>
#include <vector>


// TODO: Add interval start/finish to specify on what to apply the split
// TODO: Change from cast to string stream
template<typename T>
void split(const T& element, std::vector<std::string>& resultVector, char splittingToken = ' ') {
    if(not std::is_convertible_v<T, std::string>)
        return;

    const std::string string = Converter<T>::toString(element);
    if (string.empty()) {
        resultVector.push_back(string);
        return;
    }

    size_t startOfPart = 0;
    std::string::const_iterator it = string.begin();
    while (true) {
        size_t currentIndex = it - string.begin();
        if (it == string.end()) {
            resultVector.push_back(string.substr(startOfPart, currentIndex - startOfPart));
            break;
        }

        ++it;
        if(string[currentIndex] != splittingToken)
            continue;

        resultVector.push_back(string.substr(startOfPart, currentIndex - startOfPart));
        startOfPart = currentIndex + 1;
    }
}