#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/http/http.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <core/logging/light.hpp>
#include <core/utility/function.hpp>
#include <core/utility/converter.hpp>

namespace http_n
{
    namespace server_n
    {
        class Session : public std::enable_shared_from_this<Session> {
            using tcp = asio::ip::tcp;


            struct Private {};
        public:
            Session(const Private&, sslSocket_t&&);
            ~Session();

            asio::awaitable<void> handshake();
            asio::awaitable<std::string> read();
            std::string alpnExtension();
            void write(http_n::Response& response);
            void write(http_n::Response&& response);
            void error(network_n::Code errorCode);
            void shutdown();

            static std::shared_ptr<Session> create(sslSocket_t&& socket);

        private:
            bool m_isClosing;
            bool m_sslEstablished;
            sslSocket_t m_socket;
            std::string m_remoteEndpoint;

            inline static std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();

            void validateSSLContext() const;
            void onWriteCompleted(std::shared_ptr<std::string> stringResponse, const std::error_code& ec, std::size_t size);
        };
    }
}