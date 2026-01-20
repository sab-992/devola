#include <server/websocket/server.h>


server_n::websocket_n::Server::~Server() {}

void server_n::websocket_n::Server::addEvent(std::string event, server_n::websocket_n::CallbackFunction_t callback) {
    if (m_eventCallbacks.contains(event))
        throw std::logic_error(std::format("Event: {} already exists !", event));

    m_eventCallbacks[event] = callback;
}

void server_n::websocket_n::Server::executeEvent(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& message) {
    // TODO: Parse WS Message.
    // TODO: Verify Event exists and has callback.
    // TODO: Verify other elements (authorization maybe ?)

    // TODO: Change placeholder event.
    std::string event = "Test";
    
    if (not m_eventCallbacks.contains(event))
        // TODO: Change this for error handling. Maybe add WebSocket to custom error to be able to send response to it later in the catch block.
        return;

    m_eventCallbacks[event](connectionState, websocket, message);
}

void server_n::websocket_n::Server::getLifeCycleCallback(websocket_n::LifeCycleMsg_en Index, std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& message) {
    m_lifeCycleCallbacks[static_cast<size_t>(Index)](connectionState, websocket, message);
}

void server_n::websocket_n::Server::handleMessage(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& message) {
    if (m_trace) trace(LogType::info, "Remote IP:", connectionState->getRemoteIp());
    
    getLifeCycleCallback(websocket_n::LifeCycleMsg_en::ON_COMMUNICATION, connectionState, websocket, message);
    switch (message->type) {
        case ix::WebSocketMessageType::Message:
            executeEvent(connectionState, websocket, message);
            break;
        case ix::WebSocketMessageType::Open:
            getLifeCycleCallback(websocket_n::LifeCycleMsg_en::ON_OPENED, connectionState, websocket, message);
            break;
        case ix::WebSocketMessageType::Close:
            getLifeCycleCallback(websocket_n::LifeCycleMsg_en::ON_CLOSED, connectionState, websocket, message);
            break;
        default:
            throw std::invalid_argument("WS: Message type not supported.");
            break;
    };
}

void server_n::websocket_n::Server::run() {
    try {
        if (m_ixWSServer == nullptr)
            throw std::logic_error("WS: Server pointer is nullptr.");

        m_ixWSServer->setOnClientMessageCallback(std::bind(&server_n::websocket_n::Server::handleMessage, this, std::placeholders::_1,
                                                                                                              std::placeholders::_2,
                                                                                                              std::placeholders::_3));

        m_ixWSServer->listen();

        m_ixWSServer->disablePerMessageDeflate();

        m_ixWSServer->start();

        trace(LogType::info, std::format("Listening on port {}...", m_port));
        m_ixWSServer->wait();
    } catch(...) { /* TODO: Add Custom error class (Code + message) and Error handling */ }
}

void server_n::websocket_n::Server::setLifeCycleCallback(websocket_n::LifeCycleMsg_en type, server_n::websocket_n::CallbackFunction_t callback) {
    m_lifeCycleCallbacks[static_cast<size_t>(type)] = callback;
};

void server_n::websocket_n::Server::stop() {
    // TODO: Find a way to stop server.
    m_eventCallbacks.clear();
}

void server_n::websocket_n::Server::toggleTracing() {
    m_trace = !m_trace;
}