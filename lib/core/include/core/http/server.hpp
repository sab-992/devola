#pragma once


#include <asio.hpp>
#include <core/http/detail/session.hpp>
#include <core/http/detail/settings.hpp>
#include <core/http/http.hpp>
#include <core/logging/light.hpp>
#include <core/thread/registry.hpp>
#include <core/utility/function.hpp>
#include <functional>
#include <format>
#include <memory>
#include <unordered_map>


#define ENDPOINT(METHOD, PATH, HANDLER) \
    EndpointRegistrar(this, METHOD, PATH) = std::bind_front(HANDLER, this)

#define ENDPOINT_L(METHOD, PATH) \
    EndpointRegistrar(this, METHOD, PATH) = [&](const http_n::server_n::Session& session, const http_n::Request& request) -> asio::awaitable<http_n::Response>

namespace http_n
{
    namespace server_n
    {
        class Basic {
            using handlers_t = std::function<asio::awaitable<http_n::Response>(const Session&, const Request&)>;
            using endpoints_t = std::unordered_map<std::string, handlers_t>;
            using startSequence_t = std::function<void(Basic*)>;

            const uint8_t BASE_THREADS = 2;

        public:
            Basic(const std::string& name, uint16_t port);
            ~Basic();

            virtual std::string pathPrefix() const = 0;

            void run();
            void setStartSequence(const startSequence_t& function);
            void setThreadPoolSize(uint16_t size);
            void setEndpoint(std::string method, const std::string& endpoint, handlers_t handler);
            void stop();

        protected:
            std::vector<std::string> m_extraLogInformation;
            std::unique_ptr<Http> m_http;
            asio::io_context m_ioContext;
            asio::ssl::context m_sslContext;
            bool m_isRunning = false;
            std::unordered_map<std::string, endpoints_t> m_endpoints;
            uint16_t m_port;
            asio::signal_set m_signals;
            startSequence_t m_startSequence;
            std::vector<std::thread::id> m_threadIds;
            uint16_t m_threadPoolSize;
            asio::executor_work_guard<asio::io_context::executor_type> m_workGuard;

            inline static std::shared_ptr<log_n::Light> m_light;
            inline static std::shared_ptr<thread_n::Registry> m_threadRegistry;

            void addExtraLogInformation(const std::string& element);

            struct EndpointRegistrar {
                EndpointRegistrar(Basic* server, const std::string& method, const std::string& path)
                : m_server(server), m_method(method), m_endpoint(path) {}

                void operator=(handlers_t&& handler) {
                    m_server->setEndpoint(m_method, std::format("{}{}", m_server->pathPrefix(), m_endpoint), std::forward<handlers_t>(handler));
                }

            private:
                Basic* m_server;
                std::string m_method, m_endpoint;
            };

        private:
            asio::awaitable<void> handleClient(asio::ip::tcp::socket&& socket);
            asio::awaitable<void> listen();

            void clean();
            void readCommands() const;
            void startSequence();
            void startThreadPool();

            static int alpnSelectCallback(SSL*, const unsigned char** out, unsigned char* outlen,
                                                const unsigned char* in, unsigned int inlen, void* /*arg*/);
        };
    }
}