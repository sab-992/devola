#pragma once

#include <core/network/detail/protocol/http1_1.h>
#include <core/network/interface/protocol.h>
#include <core/network/network.h>
#include <memory>
#include <stdexcept>
#include <utility>


namespace network_n
{
    namespace protocol_n
    {
        class Factory {
        public:
            template <typename T>
            static std::unique_ptr<Protocol_i<T>> create(Protocol protocol) {
                switch (protocol)
                {
                case Protocol::HTTP1_1:
                    return std::make_unique<HTTP1_1<T>>();
                default:
                    throw std::runtime_error(std::format("Protocol #{}: Not Handled", std::to_underlying(protocol))); // TODO: Change for new error type;
                }
            }
        };
    }
}