#pragma once

#include <cstdint>


namespace log_n {
    enum class Level_en : uint8_t {
        DEBUG,
        ERROR,
        INFO,
        SPECIAL,
        WARNING
    };
}