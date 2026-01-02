#pragma once

#include <Converter.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <LogType.h>
#include <Trim.h>
#include <Rang.h>

template<typename T, typename... Args>
void Trace(std::function<std::unique_ptr<DisplayColor_i>()> DisplayColorFunction, T FirstArg, Args... OtherArgs) {
    std::string CurrentWord = Converter<T>::ToString(FirstArg);
    if (Trim(CurrentWord).empty()) {
        std::cout << rang::style::reset << std::endl;
        return ;
    }

    std::cout << DisplayColorFunction()->Color() << CurrentWord;
    if constexpr (sizeof...(Args) > 0)
        Trace<Args...>(DisplayColorFunction, OtherArgs...);
    else 
        Trace<Args...>(DisplayColorFunction, OtherArgs..., "");
}

