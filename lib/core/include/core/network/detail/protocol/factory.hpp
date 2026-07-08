#pragma once

#include <core/utility/enum.hpp>
#include <core/network/detail/protocol/http1_1/http1_1.hpp>
#include <core/network/interface/protocol.hpp>
#include <core/network/network.hpp>
#include <memory>
#include <unordered_map>


namespace network_n
{
    namespace protocol_n
    {
        class Factory {
        public:
            static std::shared_ptr<Protocol_i> create(const Protocol& protocol);
            static std::shared_ptr<Protocol_i> create(const std::string& alpnExtension);

        private:
            static const std::unordered_map<Protocol, std::shared_ptr<Protocol_i>>& getProtocols();
            static std::unordered_map<std::string, Protocol> getALPNExtensions();
            static std::unordered_map<std::string, Protocol> buildExtensionToProtocolUmap();
        };
    }
}