#pragma once

#include "core/logging/detail/rang.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>


namespace log_n {
    enum class Level_en : uint8_t {
        DEBUG,
        ERROR,
        INFO,
        SPECIAL,
        WARNING
    };

    // Spaces after the string version of the log level are there to make sure every message starts at same column of terminal.
    const std::unordered_map<log_n::Level_en,
                             std::pair<std::string, rang::fg>> LEVELS_COLOR = { { log_n::Level_en::DEBUG,   { "[DEBUG]", rang::fg::blue } },
                                                                                { log_n::Level_en::ERROR,   { "[ERROR]", rang::fg::red } },
                                                                                { log_n::Level_en::INFO,    { "[INFO] ", rang::fg::green } },
                                                                                { log_n::Level_en::SPECIAL, { "[SPEC] ", rang::fg::magenta } },
                                                                                { log_n::Level_en::WARNING, { "[WARN] ", rang::fg::yellow } } };
}