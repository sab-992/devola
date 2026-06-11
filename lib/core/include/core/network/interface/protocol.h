#pragma once

#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/detail/protocol/http1_1/body_parser.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/interface/networking.h>
#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <string>
#include <tuple>


namespace network_n
{
    namespace protocol_n
    {
        template <typename T>
        class Protocol_i : public Networking_i<T> {
        public:
            virtual ~Protocol_i() = default;

            virtual std::string alpnExtension() const = 0;
            virtual std::shared_ptr<BodyParser_i<T>> bodyParser() const = 0;
            virtual uint16_t defaultPort() const = 0;
            virtual std::shared_ptr<HeadersParser_i> headersParser() const = 0;
            virtual std::string messageToString(const network_n::Headers& headers, const network_n::Body<T>& body) const = 0;
            virtual std::string name() const = 0;
            virtual std::vector<std::string> packetize(const network_n::Headers& headers, const network_n::Body<T>& body) const = 0;
            virtual std::tuple<startLineInformation_t, Headers, Body<T>> parse(std::string_view raw) const = 0;
            virtual std::tuple<startLineInformation_t, Headers, Body<T>> parse(std::string_view rawHeaders, std::string_view rawBody) const = 0;
        };
    }
}