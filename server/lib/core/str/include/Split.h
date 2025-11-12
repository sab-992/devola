#include <string>
#include <vector>


template<typename T>
void Split(T Element, std::vector<std::string>& ResultVector, char SplittingToken = ' ') {
    if(not std::is_convertible_v<T, std::string>)
        return;

    std::string String = static_cast<std::string>(Element);
    if (String.empty())
        return;

    size_t StartOfPart = 0;
    std::string::iterator It = String.begin();
    while (true) {
        size_t CurrentIndex = It - String.begin();
        if (It == String.end()) {
            ResultVector.push_back(String.substr(StartOfPart, CurrentIndex - StartOfPart));
            break;
        }

        ++It;
        if(String[CurrentIndex] != SplittingToken)
            continue;

        ResultVector.push_back(String.substr(StartOfPart, CurrentIndex - StartOfPart));
        StartOfPart = CurrentIndex + 1;
    }
}