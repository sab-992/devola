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


namespace http_n
{
    namespace server_n
    {
        class Basic {
            using completionToken_t = std::function<http_n::Response(const Session& session, const Request& request)>;
            using endpointsUMap_t = std::unordered_map<std::string, completionToken_t>;

            struct Private_s {};

            const uint8_t BASE_THREADS = 2;
            const uint8_t THREADS_RESERVE_SIZE = 25;

        public:
            Basic(const Private_s&, uint16_t port);
            ~Basic();

            friend std::unique_ptr<Basic> std::make_unique<Basic>();

            void run();
            void setStartSequence(const std::function<void()>& function);
            void setThreadPoolSize(uint16_t size);
            void setEndpoints(std::string method, const endpointsUMap_t& endpoints);
            void stop();

            static std::unique_ptr<Basic> create(uint16_t port=443);

        protected:
            std::unique_ptr<Http> m_http;
            asio::io_context m_ioContext;
            asio::ssl::context m_sslContext;
            bool m_isRunning = false;
            std::unordered_map<std::string, std::function<void()>> m_commands;
            std::unordered_map<std::string, endpointsUMap_t> m_methodEndpoints;
            uint16_t m_port;
            asio::signal_set m_signals;
            std::function<void()> m_startSequence;
            std::vector<std::thread::id> m_threadIds;
            uint16_t m_threadPoolSize;
            asio::executor_work_guard<asio::io_context::executor_type> m_workGuard;

            inline static std::shared_ptr<log_n::Light> m_light;
            inline static std::shared_ptr<thread_n::Registry> m_threadRegistry;

        private:
            asio::awaitable<void> handleClient(asio::ip::tcp::socket&& socket);
            asio::awaitable<void> listen();

            void readCommands() const;
            void setupCommands();
            void startSequence() const;
            void startThreadPool();

            static int alpnSelectCallback(SSL*, const unsigned char** out, unsigned char* outlen,
                                                const unsigned char* in, unsigned int inlen, void* /*arg*/);
        };
    }
}