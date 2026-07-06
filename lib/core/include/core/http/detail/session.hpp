#pragma once

#include <asio.hpp>
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
            Session(tcp::socket&& socket);
            ~Session();

            asio::awaitable<std::string> read();
            asio::awaitable<void> write(const http_n::Response<std::string>& response);
            asio::awaitable<void> error(network_n::Code errorCode);

        private:
            tcp::socket m_socket;
            inline static std::shared_ptr<log_n::Light> m_light = log_n::Light::instance();
        };
    }
}