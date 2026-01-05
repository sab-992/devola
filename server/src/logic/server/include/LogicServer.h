#pragma once

#include <format>
#include <ixwebsocket/IXWebSocketServer.h>
#include <Server.h>
#include <stdexcept>
#include <string>
#include <Trace.h>
#include <WebSocketServer.h>



// TODO: Change WebSocketServer to interface WebSocketServer_i

class LogicServer: Server_n::Server_i {
public:
    template<typename T>
    LogicServer(T Address, int16_t Port=80) {
        m_WS = std::unique_ptr<Server_n::WS_n::Server>(new Server_n::WS_n::Server(Address, Port));
        SetupEvents();
    };

    template<typename T>
    LogicServer(Server_n::WS_n::TLSOptions TLSOptions, T Address, int16_t Port=443) {
        m_WS = std::unique_ptr<Server_n::WS_n::Server>(new Server_n::WS_n::Server(TLSOptions, Address, Port));
        SetupEvents();
    };

    ~LogicServer();

    void Run() override;
    void Stop() override;
    void ToggleTracing() override;
private:
    std::unique_ptr<Server_n::WS_n::Server> m_WS;

    void SetupEvents();
};