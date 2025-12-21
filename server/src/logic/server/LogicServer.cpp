#include <LogicServer.h>

LogicServer::~LogicServer() {}

void LogicServer::Run() {
    m_WS->Run();
}

void LogicServer::SetupEvents() {
    // TODO: Add Events
    m_WS->AddEvent("Test", [](WS_CALLBACKS_PARAMS) {
        Trace(LogType::Special, "Received:", Message->str);
        WebSocket.send(Message->str, Message->binary);
    });
}

void LogicServer::Stop() {}

void LogicServer::ToggleTracing() {
    m_WS->ToggleTracing();
}