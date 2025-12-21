#pragma once

#include <string>

class Server_i {
public:
    virtual ~Server_i() = default;

    virtual void Run() = 0;
    virtual void Stop() = 0;
    virtual void ToggleTracing() = 0;
};