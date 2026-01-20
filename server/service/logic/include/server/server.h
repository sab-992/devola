#pragma once

#include <core/logging/trace.h>
#include <format>
#include <ixwebsocket/IXWebSocketServer.h>
#include <server/interface/server.h>
#include <server/websocket/server.h>
#include <stdexcept>
#include <string>


// TODO: Change WebSocketServer to interface WebSocketServer_i
class LogicServer: server_n::Server_i {
public:
    template<typename T>
    LogicServer(T address, int16_t port=80) {
        m_wsServer = std::unique_ptr<server_n::websocket_n::Server>(new server_n::websocket_n::Server(address, port));
        setupEvents();
    };

    template<typename T>
    LogicServer(server_n::websocket_n::TLSOptions tlsOptions, T address, int16_t port=443) {
        m_wsServer = std::unique_ptr<server_n::websocket_n::Server>(new server_n::websocket_n::Server(tlsOptions, address, port));
        setupEvents();
    };

    ~LogicServer();

    void run() override;
    void stop() override;
    void toggleTracing() override;
private:
    std::unique_ptr<server_n::websocket_n::Server> m_wsServer;

    void setupEvents();
};