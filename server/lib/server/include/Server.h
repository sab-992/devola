#pragma once

#include <string>

namespace Server_n
{
    class Server_i {
    public:
        virtual ~Server_i() = default;

        virtual void Run() = 0;
        virtual void Stop() = 0;
        virtual void ToggleTracing() = 0;
    };
}