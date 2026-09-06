#pragma once

#include <cstdint>
#include <string>


namespace redis_n
{
    struct Options {
        std::string host;
        uint16_t port;
        std::string password;
        int database;
    };
}