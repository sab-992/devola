#include <WebSocketServer.h>


WebSocketServer::~WebSocketServer() {}

void WebSocketServer::AddEvent(std::string Event, WS::CallbackFunction_t Callback) {
    if (m_EventCallbacks.contains(Event))
        throw std::logic_error(std::format("Event: {} already exists !", Event));

    m_EventCallbacks[Event] = Callback;
}

void WebSocketServer::ExecuteEvent(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {
    // TODO: Parse WS Message.
    // TODO: Verify Event exists and has callback.
    // TODO: Verify other elements (authorization maybe ?)

    // TODO: Change placeholder event.
    std::string Event = "Test";
    
    if (not m_EventCallbacks.contains(Event))
        // TODO: Change this for error handling. Maybe add WebSocket to custom error to be able to send response to it later in the catch block.
        return;

    m_EventCallbacks[Event](ConnectionState, WebSocket, Message);
}

void WebSocketServer::GetLifeCycleCallback(WS::LifeCycleMsg_en Index, std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {
    m_LifeCycleCallbacks[static_cast<size_t>(Index)](ConnectionState, WebSocket, Message);
}

void WebSocketServer::HandleMessage(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {
    if (m_Trace) Trace(LogType::Info, "Remote IP:", ConnectionState->getRemoteIp());
    
    GetLifeCycleCallback(WS::LifeCycleMsg_en::ON_COMMUNICATION, ConnectionState, WebSocket, Message);
    switch (Message->type) {
        case ix::WebSocketMessageType::Message:
            ExecuteEvent(ConnectionState, WebSocket, Message);
            break;
        case ix::WebSocketMessageType::Open:
            GetLifeCycleCallback(WS::LifeCycleMsg_en::ON_OPENED, ConnectionState, WebSocket, Message);
            break;
        case ix::WebSocketMessageType::Close:
            GetLifeCycleCallback(WS::LifeCycleMsg_en::ON_CLOSED, ConnectionState, WebSocket, Message);
            break;
        default:
            throw std::invalid_argument("WS: Message type not supported.");
            break;
    };
}

void WebSocketServer::Run() {
    try {
        if (m_WSServer == nullptr)
            throw std::logic_error("WS: Server pointer is nullptr.");

        m_WSServer->setOnClientMessageCallback(std::bind(&WebSocketServer::HandleMessage, this, std::placeholders::_1,
                                                                                                std::placeholders::_2,
                                                                                                std::placeholders::_3));

        m_WSServer->listen();

        m_WSServer->disablePerMessageDeflate();

        m_WSServer->start();

        Trace(LogType::Info, std::format("Listening on port {}...", m_Port));
        m_WSServer->wait();
    } catch(...) { /* TODO: Add Custom error class (Code + message) and Error handling */ }
}

void WebSocketServer::SetLifeCycleCallback(WS::LifeCycleMsg_en Type, WS::CallbackFunction_t Callback) {
    m_LifeCycleCallbacks[static_cast<size_t>(Type)] = Callback;
};


void WebSocketServer::Stop() {
    // TODO: Find a way to stop server.
    m_EventCallbacks.clear();
}

void WebSocketServer::ToggleTracing() {
    m_Trace = !m_Trace;
}