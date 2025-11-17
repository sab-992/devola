#pragma once


class Server_i {
public:
    virtual ~Server_i() = default;

    virtual void Run() = 0;
};