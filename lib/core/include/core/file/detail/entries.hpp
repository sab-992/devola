#pragma once

#include <cstdint>


namespace file_n
{
    namespace flags_n
    {
        enum class Entry_en : uint16_t {
            FILE = 1 << 0,
            DIRECTORY = 1 << 1,
            ALL = FILE | DIRECTORY
        };
    }
}