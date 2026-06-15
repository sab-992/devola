#pragma once

#include <core/conversion/enum.hpp>
#include <core/network/detail/protocol/http1_1/http1_1.hpp>
#include <core/network/interface/protocol.hpp>
#include <core/network/network.hpp>
#include <memory>
#include <unordered_map>


namespace network_n
{
    namespace protocol_n
    {
        template <typename T>
        class Factory {
        public:
            static std::shared_ptr<Protocol_i<T>> create(const Protocol& protocol) {
                const auto& protocols = getProtocols();

                if (protocol == Protocol::NONE)
                    throw InvalidArgument("Cannot be NONE", "Protocol");
                else if (not protocols.contains(protocol))
                    throw Exception(std::format("Protocol #{}: Not Handled", to_underlying(protocol)));

                return protocols.at(protocol);
            }

            static std::shared_ptr<Protocol_i<T>> create(const std::string& alpnExtension) {
                const auto& alpnExtensions = getALPNExtensions();

                if (not alpnExtensions.contains(alpnExtension))
                    throw Exception(std::format("ALPN extension \"{}\": Not Handled", alpnExtension));

                return create(alpnExtensions.at(alpnExtension));
            }

        private:
            static const std::unordered_map<Protocol, std::shared_ptr<Protocol_i<T>>>& getProtocols() {
                static const std::unordered_map<Protocol, std::shared_ptr<Protocol_i<T>>> PROTOCOLS = { { Protocol::HTTP1_1, Http1_1<T>::instance() } };
                return PROTOCOLS;
            }

            static std::unordered_map<std::string, Protocol> getALPNExtensions() {
                static const std::unordered_map<std::string, Protocol> ALPN_EXTENSIONS = buildExtensionToProtocolUmap();
                return ALPN_EXTENSIONS;
            }

            static std::unordered_map<std::string, Protocol> buildExtensionToProtocolUmap() {
                std::unordered_map<std::string, Protocol> extensionsToProtocol;

                for (const auto& [protocolEnum, pointer] : getProtocols())
                    extensionsToProtocol[pointer->alpnExtension()] = protocolEnum;

                return extensionsToProtocol;
            }
        };
    }
}