#include <LogicServer.h>

LogicServer::~LogicServer() {}

void LogicServer::OnClose(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {}

void LogicServer::OnConnection(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {}

void LogicServer::OnMessage(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {
    // TODO: Find a way to send an EventName with the message (maybe manipulate the headers ?).
    std::string EventName = "Test"; 

    if (not m_OnMessageCallbacks.contains(EventName))
        // TODO: Change this for error handling. Maybe add WebSocket to custom error to be able to send response to it later in the catch block. 
        return; 
    
    m_OnMessageCallbacks[EventName](ConnectionState, WebSocket, Message);
}

void LogicServer::OnOpen(std::shared_ptr<ix::ConnectionState> ConnectionState, ix::WebSocket& WebSocket, const ix::WebSocketMessagePtr& Message) {}