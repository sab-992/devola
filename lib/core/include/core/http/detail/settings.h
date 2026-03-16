#pragma once

#include <core/network/network.h>

namespace http_n
{
    using network_n::protocol_n::Protocol;
    constexpr Protocol DEFAULT_PROTOCOL = Protocol::HTTP1_1;
}