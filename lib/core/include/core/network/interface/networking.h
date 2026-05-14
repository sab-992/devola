#pragma once

#include <asio.hpp>
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
            virtual std::string async_receive() const = 0;
            virtual void async_send() const = 0;
            virtual std::pair<Headers, Body<T>> receive(asio::ip::tcp::socket& socket) const = 0;
            virtual void send() const = 0;
        };
    }
}