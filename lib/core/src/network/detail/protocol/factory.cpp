#include <core/network/detail/version/factory.hpp>


std::unordered_map<std::string, network_n::version_n::Version> network_n::version_n::Factory::buildExtensionToProtocolUmap() {
    std::unordered_map<std::string, network_n::version_n::Version> extensionsToProtocol;

    for (const auto& [protocolEnum, pointer] : getProtocols())
        extensionsToProtocol[pointer->alpnExtension()] = protocolEnum;

    return extensionsToProtocol;
}

std::shared_ptr<network_n::version_n::Version_i> network_n::version_n::Factory::create(const Version& version) {
    const auto& protocols = getProtocols();

    if (version == Version::NONE)
        throw InvalidArgument("Cannot be NONE", "Version");
    else if (not protocols.contains(version))
        throw Exception(std::format("Version #{}: Not Handled", to_underlying(version)));

    return protocols.at(version);
}

std::shared_ptr<network_n::version_n::Version_i> network_n::version_n::Factory::create(const std::string& alpnExtension) {
    const auto& alpnExtensions = getALPNExtensions();

    if (not alpnExtensions.contains(alpnExtension))
        throw Exception(std::format("ALPN extension \"{}\": Not Handled", alpnExtension));

    return create(alpnExtensions.at(alpnExtension));
}

std::unordered_map<std::string, network_n::version_n::Version> network_n::version_n::Factory::getALPNExtensions() {
    static const std::unordered_map<std::string, Version> ALPN_EXTENSIONS = buildExtensionToProtocolUmap();
    return ALPN_EXTENSIONS;
}

const std::unordered_map<network_n::version_n::Version, std::shared_ptr<network_n::version_n::Version_i>>& network_n::version_n::Factory::getProtocols() {
    static const std::unordered_map<Version, std::shared_ptr<Version_i>> PROTOCOLS = { { Version::HTTP1_1, http_n::version_n::Http1_1::instance() } };
    return PROTOCOLS;
}