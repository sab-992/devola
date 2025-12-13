#pragma once

#include <format>
#include <functional>
#include <ixwebsocket/IXWebSocketServer.h>
#include <memory>
#include <Server.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <Trace.h>
#include <unordered_map>

#define CALLBACKS_PARAMS std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message


namespace WS {
    using CallbackFunction_t = std::function<void(std::shared_ptr<ix::ConnectionState>, ix::WebSocket&, const ix::WebSocketMessagePtr&)>;
    using CallbackMap_t = std::unordered_map<std::string, CallbackFunction_t>;

    struct TLSOptions {};
}

class WebSocketServer_i : public Server_i {
public:
    virtual ~WebSocketServer_i() = default;

    virtual void SetOnCloseCallback(WS::CallbackFunction_t Callback) = 0;
    virtual void SetOnConnectionCallback(WS::CallbackFunction_t Callback) = 0;
    virtual void SetOnOpenCallback(WS::CallbackFunction_t Callback) = 0;
    virtual void ToggleTracing() = 0;
protected:
    virtual void AddEvent(std::string EventName, WS::CallbackFunction_t Callback) = 0;
    virtual void OnClose(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void OnConnection(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void OnMessage(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void OnOpen(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void SetMainCallback() = 0;
};

// TODO: Find better ways to override Callbacks.

class WebSocketServer : public WebSocketServer_i {
public:
    template<typename T>
    WebSocketServer(T Address, int16_t Port=80) {
        Initialize<T>(Address, Port);
    };

    template<typename T>
    WebSocketServer(WS::TLSOptions TLSOptions, T Address, int16_t Port=443) {
        m_TLSOptions = TLSOptions;
        Initialize<T>(Address, Port);
    };

    ~WebSocketServer() override;

    void AddEvent(std::string EventName, WS::CallbackFunction_t Callback) override;
    void Run() override;
    void SetOnCloseCallback(WS::CallbackFunction_t Callback) override;
    void SetOnConnectionCallback(WS::CallbackFunction_t Callback) override;
    void SetOnOpenCallback(WS::CallbackFunction_t Callback) override;
    void Stop() override;
    void ToggleTracing() override;
protected:
    std::string m_Address;
    int16_t m_Port;
    WS::CallbackFunction_t m_OnCloseCallback;
    WS::CallbackFunction_t m_OnConnectionCallback;
    WS::CallbackMap_t m_OnMessageCallbacks;
    WS::CallbackFunction_t m_OnOpenCallback;
    WS::TLSOptions m_TLSOptions;
    bool m_Trace;
    std::unique_ptr<ix::WebSocketServer> m_WSServer;

    template<typename T>
    void Initialize(T Address, int16_t Port) {
        std::ostringstream Oss;
        Oss << Address;
        m_Address = Oss.str();
        m_Port = Port;
        m_WSServer = std::unique_ptr<ix::WebSocketServer>(new ix::WebSocketServer(Port, Address));
        m_Trace = false;

        m_OnCloseCallback = [](CALLBACKS_PARAMS){};
        m_OnConnectionCallback = [](CALLBACKS_PARAMS){};
        m_OnOpenCallback = [](CALLBACKS_PARAMS){};

        SetMainCallback();
    };

    void OnClose(CALLBACKS_PARAMS) override;
    void OnConnection(CALLBACKS_PARAMS) override;
    void OnMessage(CALLBACKS_PARAMS) override;
    void OnOpen(CALLBACKS_PARAMS) override;
    void SetMainCallback() override;
};