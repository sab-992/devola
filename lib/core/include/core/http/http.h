#pragma once

#include <asio.hpp>
#include <core/http/request.h>
#include <core/http/response.h>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>


namespace http_n
{
    class Http {
    public:
        Http(const Http&) = delete;
        Http& operator=(const Http&) = delete;

        Http(Http&&) = delete;
        Http& operator=(Http&&) = delete;

        ~Http() {}

        class Asynchronous;
        class Synchronous;

        // Function names are capitalized because 'delete' is a C++ reserved keyword.
        template<typename T>
        class RequestPresets {
        public:
            RequestPresets() = delete;
            ~RequestPresets() = default;

            static Request<T> Delete() { /* TODO */ return std::move(create()); }

            static Request<T> Get() {
                return std::move(create().setMethod("GET")
                                         .setHeader("Accept", RequestPresets<T>::accept())
                                         .setHeader("User-Agent", USER_AGENT)
                                         .setHeader("Connection", CLOSE_CONNECTION));
            }

            static Request<T> Head() { /* TODO */ return std::move(create()); }

            static Request<T> Options() { /* TODO */ return std::move(create()); }

            static Request<T> Patch() { /* TODO */ return std::move(create()); }

            static Request<T> Post() { /* TODO */ return std::move(create()); }

            static Request<T> Put() { /* TODO */ return std::move(create()); }

        private:
            inline static const std::string CLOSE_CONNECTION = "close";
            inline static const std::string USER_AGENT = "Devola/1.0";

            static std::string accept() { throw std::runtime_error("Not implemented"); }
            static Request<T> create() { return std::move(Request<T>().setProtocol(network_n::protocol_n::Protocol::HTTP1_1)); }
        };

        std::unique_ptr<Http::Asynchronous> async = std::make_unique<Http::Asynchronous>();
        std::unique_ptr<Http::Synchronous> sync = std::make_unique<Http::Synchronous>();

        static std::shared_ptr<Http> instance() {
            std::call_once(m_httpInitFlag, &Http::createInstance);
            return m_instance;
        }

    private:
        Http() {}

        class Asynchronous {
        public:
            Asynchronous() = default;

            friend std::unique_ptr<Asynchronous> std::make_unique<Asynchronous>();

            template<typename T>
            asio::ip::tcp::socket send(const Request<T>& request) const { /* TODO */ asio::ip::tcp::socket socket(m_ioContext); return std::move(socket); }

            template<typename T>
            Response<T> receive(asio::ip::tcp::socket& socket) const {  /* TODO */ return std::move(Response<T>()); }

            template<typename T>
            Response<T> receive(asio::ip::tcp::socket&& socket) const { return std::move(receive<T>(socket)); }
        };

        class Synchronous {
        public:
            Synchronous() = default;

            friend std::unique_ptr<Synchronous> std::make_unique<Synchronous>();

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
                using namespace asio;

                std::string message;
                error_code ec;
                read(socket,  dynamic_buffer(message), transfer_all(), ec);

                if (ec and ec != error::eof)
                    throw std::runtime_error("Error while reading response: " + ec.message());

                return std::move(Response<T>().set(message).build());
            }

            template<typename T>
            Response<T> receive(asio::ip::tcp::socket&& socket) const { return std::move(receive<T>(socket)); }
        };

        inline static std::once_flag m_httpInitFlag;
        inline static std::shared_ptr<Http> m_instance;
        inline static asio::io_context m_ioContext;

        static void createInstance() { m_instance = std::shared_ptr<Http>(new Http()); }
    };

    template<>
    std::string Http::RequestPresets<nlohmann::json>::accept() { return "application/json"; }

    template<>
    std::string Http::RequestPresets<std::string>::accept() { return "text/html"; }
}