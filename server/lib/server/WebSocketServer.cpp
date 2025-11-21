#include <WebSocketServer.h>

WebSocketServer::~WebSocketServer() {}

void WebSocketServer::AddEvent(std::string EventName, WS::CallbackFunction_t Callback) {
    if (m_OnMessageCallbacks.contains(EventName))
        throw std::logic_error(std::format("Event: {} already exists !", EventName));

    m_OnMessageCallbacks[EventName] = Callback;
}

void WebSocketServer::OnClose(CALLBACKS_PARAMS) {
    m_OnCloseCallback(ConnectionState, WebSocket, Message);
}

void WebSocketServer::OnConnection(CALLBACKS_PARAMS) {
    m_OnConnectionCallback(ConnectionState, WebSocket, Message);
}

void WebSocketServer::OnMessage(CALLBACKS_PARAMS) {
    // TODO: Find a way to send an EventName with the message (maybe manipulate the headers ?).
    std::string EventName = "Test"; 

    if (not m_OnMessageCallbacks.contains(EventName))
        // TODO: Change this for error handling. Maybe add WebSocket to custom error to be able to send response to it later in the catch block. 
        return; 
    
    m_OnMessageCallbacks[EventName](ConnectionState, WebSocket, Message);
}

void WebSocketServer::OnOpen(CALLBACKS_PARAMS) {
    m_OnOpenCallback(ConnectionState, WebSocket, Message);
}

void WebSocketServer::Run() {
    try {
        m_WSServer->listen();

        m_WSServer->disablePerMessageDeflate();

        m_WSServer->start();

        Trace(LogType::Info, std::format("Listening on port {}...", m_Port));
        m_WSServer->wait();
    } catch(...) { /* TODO: Add Custom error class (Code + message) and Error handling */ }
}

void WebSocketServer::SetMainCallback() {
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

void WebSocketServer::SetOnCloseCallback(WS::CallbackFunction_t Callback) {
    m_OnCloseCallback = Callback;
}

void WebSocketServer::SetOnConnectionCallback(WS::CallbackFunction_t Callback) {
    m_OnConnectionCallback = Callback;
}

void WebSocketServer::SetOnOpenCallback(WS::CallbackFunction_t Callback) {
    m_OnOpenCallback = Callback;
}

void WebSocketServer::Stop() {}

void WebSocketServer::ToggleTracing() {
    m_Trace = !m_Trace;
}