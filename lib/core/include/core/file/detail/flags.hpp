#pragma once

#include <fstream>


namespace file_n
{
    namespace flags_n
    {
        enum class OpenMode_en {
            APPEND = std::ios_base::app,
            BINARY = std::ios_base::binary,
            READ = std::ios_base::in,
            TRUNCATE = std::ios_base::trunc,
            WRITE = std::ios_base::out
        };
    }
}