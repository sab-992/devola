#pragma once

#include <format>
#include <ixwebsocket/IXWebSocketServer.h>
#include <Server.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <Trace.h>
#include <WebSocketServer.h>


#define CALLBACKS_PARAMS std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message

// TODO: Change WebSocketServer to interface WebSocketServer_i

class LogicServer: Server_i {
public:
    template<typename T>
    LogicServer(T Address, int16_t Port=80) {
        m_WS = std::unique_ptr<WebSocketServer>(new WebSocketServer(Address, Port));
        SetupEvents();
    };

    template<typename T>
    LogicServer(WS::TLSOptions TLSOptions, T Address, int16_t Port=443) {
        m_WS = std::unique_ptr<WebSocketServer>(new WebSocketServer(TLSOptions, Address, Port));
        
        SetupEvents();
    };

    ~LogicServer();

    void Run() override;
    void Stop() override;
    void ToggleTracing();
private:
    std::unique_ptr<WebSocketServer> m_WS;

    void SetupEvents();
};