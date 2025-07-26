#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <log-type.h>
#include <trim.h>
#include <rang.h>

template<typename T, typename... Args>
void Trace(const std::unique_ptr<DisplayColor_i>& DisplayColor, T FirstArg, Args... OtherArgs) {
    if (not FirstArg)
        return;

    std::ostringstream Oss;
    Oss << FirstArg;
    std::string CurrentWord = Oss.str();
    if (Trim(CurrentWord).empty()) {
        std::cout << rang::style::reset << std::endl;
        return ;
    }

    std::cout << DisplayColor->Color() << CurrentWord << ' ';
    if constexpr (sizeof...(Args) > 0)
        Trace<Args...>(DisplayColor, OtherArgs...);
    else 
        Trace<Args...>(DisplayColor, OtherArgs..., "");
}

