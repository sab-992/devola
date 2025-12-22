#pragma once

#include <Converter.h>
#include <format>
#include <functional>
#include <ixwebsocket/IXWebSocketServer.h>
#include <memory>
#include <Server.h>
#include <stdexcept>
#include <string>
#include <Trace.h>
#include <unordered_map>

#define WS_CALLBACKS_PARAMS std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message


namespace WS_n
{
    using CallbackFunction_t = std::function<void(std::shared_ptr<ix::ConnectionState>, ix::WebSocket&, const ix::WebSocketMessagePtr&)>;
    using CallbackMap_t = std::unordered_map<std::string, CallbackFunction_t>;

    struct TLSOptions {};

    enum class LifeCycleMsg_en : size_t {
        ON_OPENED,
        ON_COMMUNICATION,
        ON_CLOSED,
        SIZE // NEEDS TO BE LAST.
    };
}

class WebSocketServer_i : public Server_i {
public:
    virtual ~WebSocketServer_i() = default;
    virtual void SetLifeCycleCallback(WS_n::LifeCycleMsg_en Type, WS_n::CallbackFunction_t Callback) = 0;

protected:
    virtual void AddEvent(std::string EventName, WS_n::CallbackFunction_t Callback) = 0;
};

// TODO: Move Websockets into namespace and rename classes.
// TODO: Find better ways to override Callbacks.
// TODO: make the server parallelized using thread pool.

class WebSocketServer : public WebSocketServer_i {
public:
    template<typename T>
    WebSocketServer(T Address, int16_t Port=80) {
        Initialize<T>(Address, Port);
    };

    template<typename T>
    WebSocketServer(WS_n::TLSOptions TLSOptions, T Address, int16_t Port=443) {
        m_TLSOptions = TLSOptions;
        Initialize<T>(Address, Port);
    };

    ~WebSocketServer() override;

    void AddEvent(std::string Event, WS_n::CallbackFunction_t Callback) override;
    void Run() override;
    void SetLifeCycleCallback(WS_n::LifeCycleMsg_en Type, WS_n::CallbackFunction_t Callback) override;
    void Stop() override;
    void ToggleTracing() override;
protected:
    std::string m_Address;
    std::array<WS_n::CallbackFunction_t, static_cast<size_t>(WS_n::LifeCycleMsg_en::SIZE)> m_LifeCycleCallbacks;
    int16_t m_Port;
    WS_n::CallbackMap_t m_EventCallbacks;
    WS_n::TLSOptions m_TLSOptions;
    bool m_Trace;
    std::unique_ptr<ix::WebSocketServer> m_WSServer;

    void ExecuteEvent(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message);
    void GetLifeCycleCallback(WS_n::LifeCycleMsg_en Index, std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message);
    void HandleMessage(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message);

    template<typename T>
    void Initialize(T Address, int16_t Port) {
        m_Address = Converter<T>::ToString(Address);
        m_Port = Port;
        m_WSServer = std::unique_ptr<ix::WebSocketServer>(new ix::WebSocketServer(Port, Address));
        m_Trace = false;

        for (size_t i = 0; i < static_cast<size_t>(WS_n::LifeCycleMsg_en::SIZE); ++i)
            m_LifeCycleCallbacks[i] = [](WS_CALLBACKS_PARAMS){};
    };
};