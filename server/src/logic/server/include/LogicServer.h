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

class LogicServer : public WebSocketServer_c {
public:
    template<typename T>
    LogicServer(T Address, int16_t Port=80) {
        Initialize<T>(Address, Port);
    };

    template<typename T>
    LogicServer(WS::TLSOptions TLSOptions, T Address, int16_t Port=443) {
        m_TLSOptions = TLSOptions;
        Initialize<T>(Address, Port);
    };

    ~LogicServer() override;
private:
    template<typename T>
    void Initialize(T Address, int16_t Port) {
        m_Address = Address;
        m_Port = Port;
        m_WSServer = std::unique_ptr<ix::WebSocketServer>(new ix::WebSocketServer(Port, Address));
        m_Trace = false;

        SetMainCallback();
    };

    void OnClose(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) override;
    void OnConnection(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) override;
    void OnMessage(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) override;
    void OnOpen(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) override;
};