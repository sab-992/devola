#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/network.hpp>
#include <string>


namespace network_n
{
    class Networking_i {
    public:
        virtual asio::awaitable<std::pair<Headers, Body>> async_receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const = 0;
        virtual asio::awaitable<void> async_send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const = 0;
        virtual std::pair<Headers, Body> receive(asio::ssl::stream<asio::ip::tcp::socket>&  socket) const = 0;
        virtual void send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const = 0;
    };
}