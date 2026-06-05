#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/exception.h>
#include <core/http/request.h>
#include <core/http/response.h>
#include <core/utility/singleton.h>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>


namespace http_n
{
    class Http {
    public:
        Http(asio::io_context* ctx) : m_ioContext(ctx) {}
        ~Http() = default;

        #ifdef DEBUG_MODE_ENABLED
            void disablePeerVerification() { m_sslMode = asio::ssl::verify_none; }
        #endif

        template<typename T>
        asio::awaitable<Response<T>> async_receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const {
            const std::string& alpnExtension = readALPNExtension(socket);

            auto protocol = network_n::protocol_n::Factory<T>::create(alpnExtension);

            const auto& [headers, body] = co_await protocol->async_receive(socket);

            co_return Response<T>(headers, body);
        }

        template<typename T>
        asio::awaitable<Response<T>> async_receive(asio::ssl::stream<asio::ip::tcp::socket>&& socket) const { return async_receive<T>(socket); }

        template<typename T>
        asio::awaitable<asio::ssl::stream<asio::ip::tcp::socket>> async_send(const Request<T>& request) {
            if (not m_ioContext)
                throw Exception("m_ioContext is nullptr");

            using namespace asio;
            using namespace asio::ip;

            auto protocol = request.protocol();

            asio::ssl::stream<asio::ip::tcp::socket> socket = prepareSSLHandshake(protocol, request.url());

            tcp::resolver resolver(*m_ioContext);
            co_await async_connect(socket.lowest_layer(), resolver.resolve(request.url(), std::to_string(request.port())));

            co_await socket.async_handshake(asio::ssl::stream_base::client);

            co_await request.protocol()->async_send(socket, request.toString());

            co_return std::move(socket);
        }

        template<typename T>
        asio::ssl::stream<asio::ip::tcp::socket> send(const Request<T>& request) {
            if (not m_ioContext)
                throw Exception("m_ioContext is nullptr");

            using namespace asio;
            using namespace asio::ip;

            auto protocol = request.protocol();

            asio::ssl::stream<asio::ip::tcp::socket> socket = prepareSSLHandshake(protocol, request.url());

            tcp::resolver resolver(*m_ioContext);
            connect(socket.lowest_layer(), resolver.resolve(request.url(), std::to_string(request.port())));

            socket.handshake(asio::ssl::stream_base::client);

            protocol->send(socket, request.toString());

            return std::move(socket);
        }

        template<typename Function, typename Callback>
        auto spawn(Function&& function, Callback&& callback) {
            if (not m_ioContext)
                throw Exception("m_ioContext is nullptr");

            return asio::co_spawn(*m_ioContext, std::forward<Function>(function), std::forward<Callback>(callback));
        }

        template<typename T>
        Response<T> receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const {
            const std::string& alpnExtension = readALPNExtension(socket);

            auto protocol = network_n::protocol_n::Factory<T>::create(alpnExtension);

            const auto& [headers, body] = protocol->receive(socket);

            return Response<T>(headers, body);
        }

        template<typename T>
        Response<T> receive(asio::ssl::stream<asio::ip::tcp::socket>&& socket) const { return receive<T>(socket); }

    private:
        asio::io_context* m_ioContext;
        int m_sslMode = asio::ssl::verify_peer;

        void negotiateAlpnExtension(asio::ssl::context& context, std::string_view extension) const {
            const std::string& prefixedProtos = std::format("{}{}", static_cast<char>(extension.size()), extension);
            const unsigned char* protos = reinterpret_cast<const unsigned char*>(prefixedProtos.data());
            SSL_CTX_set_alpn_protos(context.native_handle(), protos, sizeof(protos) + 1);
        }

        template<typename T>
        asio::ssl::stream<asio::ip::tcp::socket> prepareSSLHandshake(std::shared_ptr<network_n::protocol_n::Protocol_i<T>> protocol, const std::string& hostName) {
            if (not m_ioContext)
                throw Exception("m_ioContext is nullptr");

            namespace ssl = asio::ssl;
            using namespace asio::ip;

            ssl::context sslContext(ssl::context::tls_client);

            negotiateAlpnExtension(sslContext, protocol->alpnExtension());

            sslContext.set_verify_mode(m_sslMode);
            sslContext.set_default_verify_paths();

            asio::ssl::stream<tcp::socket> socket(*m_ioContext, sslContext);
            SSL_set_tlsext_host_name(socket.native_handle(), hostName.c_str());

            return std::move(socket);
        }

        std::string readALPNExtension(asio::ssl::stream<asio::ip::tcp::socket>& socket) const {
            const unsigned char* alpn;
            unsigned int alpn_len;

            SSL* ssl = socket.native_handle();
            SSL_get0_alpn_selected(ssl, &alpn, &alpn_len);

            if (not alpn or alpn_len <= 0)
                throw Exception("No ALPN extension negotiated");

            return std::string(reinterpret_cast<const char*>(alpn), alpn_len);
        }
    };
}