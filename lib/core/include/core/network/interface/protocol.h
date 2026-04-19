#pragma once

#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/detail/protocol/http1_1/body_parser.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/detail/headers.h>
#include <string>
#include <utility>


namespace network_n
{
    namespace protocol_n
    {
        template <typename T>
        class Protocol_i {
        public:
            virtual ~Protocol_i() = default;

            virtual std::string build(const network_n::Headers& headers, const network_n::Body<T>& body) const = 0;
            virtual std::unique_ptr<BodyParser_i<T>> bodyParser() const = 0;
            virtual uint16_t defaultPort() const = 0;
            virtual std::unique_ptr<HeadersParser_i> headersParser() const = 0;
            virtual std::string name() const = 0;
            virtual std::pair<network_n::Headers, network_n::Body<T>> parse(const std::string& raw) const = 0;
            virtual std::unordered_map<std::string, std::string> parseStartLine(const std::string& startLine) const = 0;
            virtual std::string messageToString(const network_n::Headers& headers, const network_n::Body<T>& body) const = 0;
        };
    }
}