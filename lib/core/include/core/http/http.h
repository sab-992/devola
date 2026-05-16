#pragma once

#include <asio.hpp>
#include <core/exception.h>
#include <core/http/request.h>
#include <core/http/response.h>
#include <core/utility/singleton.h>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>


// TODO: Find a way to detect protocol using SSL/TLS and change it in the HTTP message.
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
        asio::ip::tcp::socket send(const Request<T>& request) const {
            using namespace asio;

            ip::tcp::resolver resolver(m_ioContext);
            ip::tcp::socket socket(m_ioContext);
            connect(socket, resolver.resolve(request.url(), std::to_string(request.port())));
            write(socket, buffer(request.toString()));
            return std::move(socket);
        }

        template<typename T>
        Response<T> receive(asio::ip::tcp::socket& socket) const {
            auto protocol = network_n::protocol_n::Factory::create<T>(http_n::DEFAULT_PROTOCOL);

            const auto& [headers, body] = protocol->receive(socket);

            return Response<T>(std::move(headers), std::move(body));
        }

        template<typename T>
        Response<T> receive(asio::ip::tcp::socket&& socket) const { return receive<T>(socket); }

    private:
        inline static asio::io_context m_ioContext;
    };
}