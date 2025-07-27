#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <sstream>
#include <string>

#include <LogType.h>
#include <Trim.h>
#include <Rang.h>

template<typename T, typename... Args>
void Trace(std::function<std::unique_ptr<DisplayColor_i>()> DisplayColorFunction, T FirstArg, Args... OtherArgs) {
    if (not FirstArg)
        return;

    std::ostringstream Oss;
    Oss << FirstArg;
    std::string CurrentWord = Oss.str();
    if (Trim(CurrentWord).empty()) {
        std::cout << rang::style::reset << std::endl;
        return ;
    }

    std::cout << DisplayColorFunction()->Color() << CurrentWord << ' ';
    if constexpr (sizeof...(Args) > 0)
        Trace<Args...>(DisplayColorFunction, OtherArgs...);
    else 
        Trace<Args...>(DisplayColorFunction, OtherArgs..., "");
}

