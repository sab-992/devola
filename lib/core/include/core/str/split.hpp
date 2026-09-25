#pragma once

#include <string>
#include <vector>


// Splits the string at the given range [start, end) using the specified delimiter and stores the results in the provided vector.
void split(std::string_view element, std::vector<std::string>& resultVector, std::string_view splittingToken=" ", size_t start=0, size_t end=std::string::npos);
std::vector<std::string> split(std::string_view element, std::string_view splittingToken=" ", size_t start=0, size_t end=std::string::npos);