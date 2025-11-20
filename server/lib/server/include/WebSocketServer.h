#include <functional>
#include <ixwebsocket/IXWebSocketServer.h>
#include <memory>
#include <Server.h>
#include <string>
#include <Trace.h>
#include <unordered_map>


namespace WS {
    using CallbackFunction_t = std::function<void(std::shared_ptr<ix::ConnectionState>, ix::WebSocket&, const ix::WebSocketMessagePtr&)>;
    using CallbackMap_t = std::unordered_map<std::string, CallbackFunction_t>;

    struct TLSOptions {};
}

class WebSocketServer_i : public Server_i {
public:
    virtual ~WebSocketServer_i() = default;

    virtual void AddEvent(std::string EventName, WS::CallbackFunction_t Callback) = 0;
    virtual void ToggleTracing() = 0;
protected:
    virtual void OnClose(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void OnConnection(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void OnMessage(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void OnOpen(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) = 0;
    virtual void SetMainCallback() = 0;
};

class WebSocketServer_c : public WebSocketServer_i {
public:
    ~WebSocketServer_c() override;

    void AddEvent(std::string EventName, WS::CallbackFunction_t Callback) override;
    void Run() override;
    void Stop() override;
    void ToggleTracing() override;
protected:
    std::string m_Address;
    int16_t m_Port;
    WS::CallbackMap_t m_OnMessageCallbacks;
    WS::TLSOptions m_TLSOptions;
    bool m_Trace;
    std::unique_ptr<ix::WebSocketServer> m_WSServer;

    virtual void SetMainCallback() override;
};