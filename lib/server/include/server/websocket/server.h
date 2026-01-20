#pragma once

#include <core/logging.h>
#include <core/utils/converter.h>
#include <format>
#include <functional>
#include <ixwebsocket/IXWebSocketServer.h>
#include <memory>
#include <server/interface/server.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

#define WS_CALLBACKS_PARAMS std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& message


namespace server_n
{
    namespace websocket_n
    {
        // TODO: Move these to a setting file
        using CallbackFunction_t = std::function<void(std::shared_ptr<ix::ConnectionState>, ix::WebSocket&, const ix::WebSocketMessagePtr&)>;
        using CallbackMap_t = std::unordered_map<std::string, CallbackFunction_t>;

        struct TLSOptions {};

        enum class LifeCycleMsg_en : size_t {
            ON_OPENED,
            ON_COMMUNICATION,
            ON_CLOSED,
            SIZE // NEEDS TO BE LAST.
        };

        class Server_i : public server_n::Server_i {
        public:
            virtual ~Server_i() = default;
            virtual void setLifeCycleCallback(server_n::websocket_n::LifeCycleMsg_en type, server_n::websocket_n::CallbackFunction_t callback) = 0;

        protected:
            virtual void addEvent(std::string event, server_n::websocket_n::CallbackFunction_t callback) = 0;
        };

        // TODO: Move Websockets into namespace and rename classes.
        // TODO: Move to protected section to private.
        // TODO: make the server parallelized using thread pool.

        class Server : public server_n::websocket_n::Server_i {
        public:
            template<typename T>
            Server(T Address, int16_t Port=80) {
                initialize<T>(Address, Port);
            };

            template<typename T>
            Server(websocket_n::TLSOptions tlsOptions, T address, int16_t port=443) {
                m_tlsOptions = tlsOptions;
                initialize<T>(address, port);
            };

            ~Server() override;

            void addEvent(std::string event, server_n::websocket_n::CallbackFunction_t callback) override;
            void run() override;
            void setLifeCycleCallback(server_n::websocket_n::LifeCycleMsg_en type, server_n::websocket_n::CallbackFunction_t callback) override;
            void stop() override;
            void toggleTracing() override;
        protected:
            std::string m_address;
            std::array<server_n::websocket_n::CallbackFunction_t, static_cast<size_t>(server_n::websocket_n::LifeCycleMsg_en::SIZE)> m_lifeCycleCallbacks;
            int16_t m_port;
            server_n::websocket_n::CallbackMap_t m_eventCallbacks;
            server_n::websocket_n::TLSOptions m_tlsOptions;
            bool m_trace;
            std::unique_ptr<ix::WebSocketServer> m_ixWSServer;

            void executeEvent(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& message);
            void getLifeCycleCallback(server_n::websocket_n::LifeCycleMsg_en index, std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& message);
            void handleMessage(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& message);

            template<typename T>
            void initialize(T address, int16_t port) {
                m_address = Converter<T>::toString(address);
                m_port = port;
                m_ixWSServer = std::unique_ptr<ix::WebSocketServer>(new ix::WebSocketServer(port, address));
                m_trace = false;

                for (size_t i = 0; i < static_cast<size_t>(server_n::websocket_n::LifeCycleMsg_en::SIZE); ++i)
                    m_lifeCycleCallbacks[i] = [](WS_CALLBACKS_PARAMS){};
            };
        };
    }
}