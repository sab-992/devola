#pragma once

#include <core/logging/detail/rang.h>
#include <core/logging/log_type.h>
#include <core/str/trim.h>
#include <core/utils/converter.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>


// Rename function
template<typename T, typename... Args>
void trace(std::function<std::unique_ptr<DisplayColor_i>()> displayColorFunction, T firstArg, Args... otherArgs) {
    const std::string currentWord = Converter<T>::toString(firstArg);
    if (trim(currentWord).empty()) {
        std::cout << rang::style::reset << std::endl;
        return ;
    }

    std::cout << displayColorFunction()->color() << currentWord;
    if constexpr (sizeof...(Args) > 0)
        trace<Args...>(displayColorFunction, otherArgs...);
    else 
        trace<Args...>(displayColorFunction, otherArgs..., "");
}

