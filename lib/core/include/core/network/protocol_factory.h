#pragma once

#include <core/conversion/enum.h>
#include <core/network/detail/protocol/http1_1/http1_1.h>
#include <core/network/interface/protocol.h>
#include <core/network/network.h>
#include <memory>
#include <stdexcept>


namespace network_n
{
    namespace protocol_n
    {
        class Factory {
        public:
            template <typename T>
            static std::shared_ptr<Protocol_i<T>> create(const Protocol& protocol) {
                switch (protocol)
                {
                case Protocol::HTTP1_1:
                    return Http1_1<T>::instance();
                case Protocol::NONE:
                    throw InvalidArgument("Cannot be NONE", "Protocol");
                default:
                    throw Exception(std::format("Protocol #{}: Not Handled", to_underlying(protocol)));
                }
            }
        };
    }
}