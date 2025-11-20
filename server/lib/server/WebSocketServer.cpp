#include <WebSocketServer.h>

WebSocketServer_c::~WebSocketServer_c() {}

void WebSocketServer_c::AddEvent(std::string EventName, WS::CallbackFunction_t Callback) {
    if (m_OnMessageCallbacks.contains(EventName))
        throw std::logic_error(std::format("Event: {} already exists !", EventName));

    m_OnMessageCallbacks[EventName] = Callback;
}

void WebSocketServer_c::Run() {
    try {
        m_WSServer->listen();

        m_WSServer->disablePerMessageDeflate();

        m_WSServer->start();

        Trace(LogType::Info, std::format("Listening on port {}...", m_Port));
        m_WSServer->wait();
    } catch(...) { /* TODO: Add Custom error class (Code + message) and Error handling */ }
}

void WebSocketServer_c::SetMainCallback() {
    if (m_WSServer == nullptr)
        throw std::logic_error("WS: Server pointer is nullptr.");

    m_WSServer->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {
        if (m_Trace) Trace(LogType::Info, "Remote IP:", ConnectionState->getRemoteIp());
        
        OnConnection(ConnectionState, WebSocket, Message);
        switch (Message->type) {
            case ix::WebSocketMessageType::Close:
                OnClose(ConnectionState, WebSocket, Message);
                break;
            case ix::WebSocketMessageType::Message:
                OnMessage(ConnectionState, WebSocket, Message);
                break;
            case ix::WebSocketMessageType::Open:
                OnOpen(ConnectionState, WebSocket, Message);
                break;
            default:
                throw std::invalid_argument("WS: Message type not supported.");
                break;
        };
    });
}

void WebSocketServer_c::Stop() {}

void WebSocketServer_c::ToggleTracing() {
    m_Trace = !m_Trace;
}