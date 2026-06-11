#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <string>


namespace network_n
{
    namespace protocol_n
    {
        template <typename T>
        class Networking_i {
        public:
            virtual asio::awaitable<std::pair<Headers, Body<T>>> async_receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const = 0;
            virtual asio::awaitable<void> async_send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const = 0;
            virtual std::pair<Headers, Body<T>> receive(asio::ssl::stream<asio::ip::tcp::socket>&  socket) const = 0;
            virtual void send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const = 0;
        };
    }
}