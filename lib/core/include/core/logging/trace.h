#pragma once

#include <core/logging/detail/rang.h>
#include <core/logging/log_type.h>
#include <core/str/trim.h>
#include <core/conversion/converter.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>


// TODO: Rename function
// TODO: Change for a std::string_view
// TODO: Rework the way tracing work.
// Maybe class LowLevel --> creates a html file with everything.
// Maybe class HighLevel --> Basic colored and formatted strings with time and other things.
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

