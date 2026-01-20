#include <server/server.h>

LogicServer::~LogicServer() {}

void LogicServer::run() {
    m_wsServer->run();
}

void LogicServer::setupEvents() {
    // TODO: Add Events
    m_wsServer->addEvent("Test", [](WS_CALLBACKS_PARAMS) {
        trace(LogType::special, "Received:", message->str);
        websocket.send(message->str, message->binary);
    });
}

void LogicServer::stop() {}

void LogicServer::toggleTracing() {
    m_wsServer->toggleTracing();
}