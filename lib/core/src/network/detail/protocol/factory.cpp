#include <core/network/detail/protocol/factory.hpp>


std::unordered_map<std::string, network_n::protocol_n::Protocol> network_n::protocol_n::Factory::buildExtensionToProtocolUmap() {
    std::unordered_map<std::string, network_n::protocol_n::Protocol> extensionsToProtocol;

    for (const auto& [protocolEnum, pointer] : getProtocols())
        extensionsToProtocol[pointer->alpnExtension()] = protocolEnum;

    return extensionsToProtocol;
}

std::shared_ptr<network_n::protocol_n::Protocol_i> network_n::protocol_n::Factory::create(const Protocol& protocol) {
    const auto& protocols = getProtocols();

    if (protocol == Protocol::NONE)
        throw InvalidArgument("Cannot be NONE", "Protocol");
    else if (not protocols.contains(protocol))
        throw Exception(std::format("Protocol #{}: Not Handled", to_underlying(protocol)));

    return protocols.at(protocol);
}

std::shared_ptr<network_n::protocol_n::Protocol_i> network_n::protocol_n::Factory::create(const std::string& alpnExtension) {
    const auto& alpnExtensions = getALPNExtensions();

    if (not alpnExtensions.contains(alpnExtension))
        throw Exception(std::format("ALPN extension \"{}\": Not Handled", alpnExtension));

    return create(alpnExtensions.at(alpnExtension));
}

std::unordered_map<std::string, network_n::protocol_n::Protocol> network_n::protocol_n::Factory::getALPNExtensions() {
    static const std::unordered_map<std::string, Protocol> ALPN_EXTENSIONS = buildExtensionToProtocolUmap();
    return ALPN_EXTENSIONS;
}

const std::unordered_map<network_n::protocol_n::Protocol, std::shared_ptr<network_n::protocol_n::Protocol_i>>& network_n::protocol_n::Factory::getProtocols() {
    static const std::unordered_map<Protocol, std::shared_ptr<Protocol_i>> PROTOCOLS = { { Protocol::HTTP1_1, Http1_1::instance() } };
    return PROTOCOLS;
}