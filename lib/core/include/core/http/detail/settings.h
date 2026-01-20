#pragma once

#include <string>


namespace http_n
{
    constexpr int TCP_WINDOW_SIZE = 16384;
    const std::string PROTOCOL = "HTTP/1.1";
    const std::string KEEP_CONNECTION_ALIVE = "keep-alive";
    const std::string CLOSE_CONNECTION = "close";
    const std::string USER_AGENT = "Devola/1.0";
    const std::string TRANSFER_ENCODING = "Transfer-Encoding";
    const std::string CHUNKED = "chunked";
}