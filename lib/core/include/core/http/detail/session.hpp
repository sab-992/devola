#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/http/http.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <core/logging/light.hpp>
#include <core/utility/converter.hpp>

namespace http_n
{
    namespace server_n
    {
        class Session {
            using tcp = asio::ip::tcp;

        public:
            Session(sslSocket_t&&);
            ~Session();

            asio::awaitable<void> handshake();
            asio::awaitable<std::string> read();
            std::string alpnExtension();
            asio::awaitable<void> write(const http_n::Response& response);
            asio::awaitable<void> error(network_n::Code errorCode);

        private:
            bool m_sslEstablished;
            sslSocket_t m_socket;
            std::string m_remoteEndpoint;

            inline static std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();

            void validateSSLContext() const;
        };
    }
}