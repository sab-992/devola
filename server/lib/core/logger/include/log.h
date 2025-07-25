#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <log-type.h>
#include <trim.h>

template<typename T, typename... Args>
void Log(T FirstArg, Args... OtherArgs) {
    if (not FirstArg)
        return;

    std::ostringstream Oss;
    Oss << FirstArg;
    std::string CurrentWord = Oss.str();
    if (Trim(CurrentWord).empty()) {
        std::cout << std::endl;
        return ;
    }

    std::cout << CurrentWord << ' ';
    if constexpr (sizeof...(Args) > 0)
        Log<Args...>(OtherArgs...);
    else 
        Log<Args...>(OtherArgs..., "");
}
