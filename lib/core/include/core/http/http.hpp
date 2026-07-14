#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/exception.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <core/utility/singleton.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>


namespace http_n
{
    class Http {
        using Protocol_i = network_n::protocol_n::Protocol_i;

    public:
        Http(asio::io_context* ctx);
        ~Http() = default;

        friend std::unique_ptr<Http> std::make_unique<Http>();

        #ifdef DEBUG_MODE_ENABLED
            void disablePeerVerification();
        #endif

        asio::awaitable<Response> async_receive(sslSocket_t& socket) const;
        asio::awaitable<Response> async_receive(sslSocket_t&& socket) const;
        asio::awaitable<sslSocket_t> async_send(const Request& request) const;

        Response receive(sslSocket_t& socket) const;
        Response receive(sslSocket_t&& socket) const;
        sslSocket_t send(const Request& request) const;

        // The function "function" needs to take its parameters by value. Otherwise, it is possible
        // that the object would be dangling by the time the async function would run.
        template<typename Function, typename CompletionToken>
        auto spawn(Function&& function, CompletionToken&& token) const {
            if (not m_ioContext)
                throw Exception("m_ioContext is nullptr");

            return asio::co_spawn(*m_ioContext, std::forward<Function>(function), std::forward<CompletionToken>(token));
        }

        static std::string readALPNExtension(sslSocket_t& socket);

    private:
        asio::io_context* m_ioContext;
        int m_sslMode = asio::ssl::verify_peer;

        void negotiateAlpnExtension(asio::ssl::context& context, std::string_view extension) const;
        sslSocket_t prepareSSLHandshake(std::shared_ptr<Protocol_i> protocol, const std::string& hostName) const;
    };
}