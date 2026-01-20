#pragma once

#include <string>


namespace server_n
{
    class Server_i {
    public:
        virtual ~Server_i() = default;

        virtual void run() = 0;
        virtual void stop() = 0;
        virtual void toggleTracing() = 0;
    };
}