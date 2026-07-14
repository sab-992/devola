#pragma once

#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/http/detail/protocol/http1_1/body_parser.hpp>
#include <core/http/detail/protocol/http1_1/headers_parser.hpp>
#include <core/network/interface/networking.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/network.hpp>
#include <string>
#include <tuple>


namespace network_n
{
    namespace protocol_n
    {
        class Protocol_i : public network_n::Networking_i {
        public:
            virtual ~Protocol_i() = default;

            virtual std::string alpnExtension() const = 0;
            virtual std::shared_ptr<network_n::protocol_n::BodyParser_i> bodyParser() const = 0;
            virtual uint16_t defaultPort() const = 0;
            virtual std::shared_ptr<network_n::protocol_n::HeadersParser_i> headersParser() const = 0;
            virtual std::string messageToString(const network_n::Headers& headers, const network_n::Body& body) const = 0;
            virtual std::string name() const = 0;
            virtual std::vector<std::string> packetize(const network_n::Headers& headers, const network_n::Body& body) const = 0;
            virtual std::tuple<startLineInformation_t, network_n::Headers, network_n::Body> parse(std::string_view raw) const = 0;
            virtual std::tuple<startLineInformation_t, network_n::Headers, network_n::Body> parse(std::string_view rawHeaders, std::string_view rawBody) const = 0;
        };
    }
}