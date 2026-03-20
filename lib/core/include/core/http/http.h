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
    template<typename T>
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
        class RequestPresets {
        public:
            RequestPresets() = delete;
            ~RequestPresets() = default;

            static Request<T> Delete() { /* TODO */ return std::move(create()); }

            static Request<T> Get() {
                return std::move(create().setMethod("GET")
                                         .setHeader("Accept", Http<T>::accept())
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

            static Request<T> create() { return std::move(Request<T>().setProtocol(network_n::protocol_n::Protocol::HTTP1_1)); }
        };

        std::unique_ptr<Http<T>::Asynchronous> async = std::make_unique<Http<T>::Asynchronous>();
        std::unique_ptr<Http<T>::Synchronous> sync = std::make_unique<Http<T>::Synchronous>();

        static std::shared_ptr<Http<T>> instance() {
            std::call_once(m_httpInitFlag, &Http::createInstance);
            return m_instance;
        }

    private:
        Http() {}

        class Asynchronous {
        public:
            Asynchronous() = default;

            friend std::unique_ptr<Asynchronous> std::make_unique<Asynchronous>();

            asio::ip::tcp::socket send(const Request<T>& request) const { /* TODO */ asio::ip::tcp::socket socket(m_ioContext); return std::move(socket); }
            Response<T> receive(asio::ip::tcp::socket& socket) const {  /* TODO */ return std::move(Response<T>()); }
        };

        class Synchronous {
        public:
            Synchronous() = default;

            friend std::unique_ptr<Synchronous> std::make_unique<Synchronous>();

            asio::ip::tcp::socket send(const Request<T>& request) const {
                asio::ip::tcp::resolver resolver(m_ioContext);
                asio::ip::tcp::socket socket(m_ioContext);
                asio::connect(socket, resolver.resolve(request.url(), std::to_string(request.port())));
                asio::write(socket, asio::buffer(request.toString()));
                return std::move(socket);
            }

            Response<T> receive(asio::ip::tcp::socket& socket) const {
                using namespace asio;

                std::string message;
                error_code ec;
                read(socket,  dynamic_buffer(message), transfer_all(), ec);

                if (ec and ec != error::eof)
                    throw std::runtime_error("Error while reading response: " + ec.message());

                return std::move(Response<T>()); // TODO: pass the message to build response.
            }

            Response<T> receive(asio::ip::tcp::socket&& socket) const { return std::move(receive(socket)); }
        };

        inline static std::once_flag m_httpInitFlag;
        inline static std::shared_ptr<Http<T>> m_instance;
        inline static asio::io_context m_ioContext;

        static std::string accept() { throw std::runtime_error("Not implemented"); }
        static void createInstance() { m_instance = std::shared_ptr<Http<T>>(new Http<T>()); }
    };

    template<>
    std::string Http<nlohmann::json>::accept() { return "application/json"; }

    template<>
    std::string Http<std::string>::accept() { return "text/html"; }
}