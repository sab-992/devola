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
    class Http : public Singleton<Http> {
    public:
        Http(const Singleton<Http>::Creator_s&) {}

        Http(const Http&) = delete;
        Http& operator=(const Http&) = delete;

        Http(Http&&) = delete;
        Http& operator=(Http&&) = delete;

        ~Http() = default;

        template<typename T>
        Response<T> async_receive(asio::ip::tcp::socket& socket) const { return Response<T>(); }

        template<typename T>
        Response<T> async_receive(asio::ip::tcp::socket&& socket) const { return receive<T>(socket); }

        template<typename T>
        asio::ip::tcp::socket async_send(const Request<T>& request) const {}


        template<typename T>
        asio::ssl::stream<asio::ip::tcp::socket> send(const Request<T>& request) const {
            using namespace asio;
            using namespace asio::ip;

            auto protocol = request.protocol();

            asio::ssl::stream<asio::ip::tcp::socket> socket = prepareSSLHandshake(protocol, request.url());

            tcp::resolver resolver(m_ioContext);
            connect(socket.lowest_layer(), resolver.resolve(request.url(), std::to_string(request.port())));

            socket.handshake(asio::ssl::stream_base::client);

            protocol->send(socket, request.toString());

            return std::move(socket);
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
        inline static asio::io_context m_ioContext;

        void negotiateAlpnExtension(asio::ssl::context& context, std::string_view extension) const {
            const std::string& prefixedProtos = std::format("{}{}", static_cast<char>(extension.size()), extension);
            const unsigned char* protos = reinterpret_cast<const unsigned char*>(prefixedProtos.data());
            SSL_CTX_set_alpn_protos(context.native_handle(), protos, sizeof(protos) + 1);
        }

        template<typename T>
        asio::ssl::stream<asio::ip::tcp::socket> prepareSSLHandshake(std::shared_ptr<network_n::protocol_n::Protocol_i<T>> protocol, const std::string& hostName) const {
            namespace ssl = asio::ssl;
            using namespace asio::ip;

            ssl::context sslContext(ssl::context::tls_client);

            negotiateAlpnExtension(sslContext, protocol->alpnExtension());

            sslContext.set_verify_mode(ssl::verify_peer);
            sslContext.set_default_verify_paths();

            asio::ssl::stream<tcp::socket> socket(m_ioContext, sslContext);
            SSL_set_tlsext_host_name(socket.native_handle(), hostName.c_str());

            return std::move(socket);
        }

        std::string readALPNExtension(asio::ssl::stream<asio::ip::tcp::socket>& socket) const {
            const unsigned char* alpn;
            unsigned int alpn_len;

            SSL* ssl = socket.native_handle();
            SSL_get0_alpn_selected(ssl, &alpn, &alpn_len);

            if (not alpn or alpn_len <= 0)
                throw Exception("No ALPN extension negotiiated");

            return std::string(reinterpret_cast<const char*>(alpn), alpn_len);
        }
    };
}