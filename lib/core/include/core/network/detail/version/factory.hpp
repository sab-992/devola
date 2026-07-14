#pragma once

#include <core/http/detail/version/http1_1/http1_1.hpp>
#include <core/network/interface/version.hpp>
#include <core/network/network.hpp>
#include <core/utility/enum.hpp>
#include <memory>
#include <unordered_map>


namespace network_n
{
    namespace version_n
    {
        class Factory {
            using Version_i = network_n::version_n::Version_i;

        public:
            static std::shared_ptr<Version_i> create(const Version& version);
            static std::shared_ptr<Version_i> create(const std::string& alpnExtension);

        private:
            static const std::unordered_map<Version, std::shared_ptr<Version_i>>& getProtocols();
            static std::unordered_map<std::string, Version> getALPNExtensions();
            static std::unordered_map<std::string, Version> buildExtensionToProtocolUmap();
        };
    }
}