#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/network/network.hpp>


namespace http_n
{
    constexpr auto DEFAULT_PROTOCOL = network_n::protocol_n::Protocol::HTTP1_1;
    using sslSocket_t = asio::ssl::stream<asio::ip::tcp::socket>;
}