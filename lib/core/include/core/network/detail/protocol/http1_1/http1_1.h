#pragma once

#include <core/exception.h>
#include <core/network/detail/protocol/http1_1/body_parser.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/interface/protocol.h>
#include <format>
#include <memory>
#include <string>
#include <tuple>


namespace network_n
{
    namespace protocol_n
    {
        template<typename T>
        class HTTP1_1 : public Protocol_i<T> {
        public:
            HTTP1_1() {}

            friend std::unique_ptr<HTTP1_1> std::make_unique<HTTP1_1>();

            std::string alpn() const override { return "http/1.1"; };

            std::shared_ptr<BodyParser_i<T>> bodyParser() const override { return http1_1_n::BodyParser<T>::instance(); }

            uint16_t defaultPort() const override { return DEFAULT_PORT; }

            std::shared_ptr<HeadersParser_i> headersParser() const override { return http1_1_n::HeadersParser::instance(); }

            // Used for debugging + tests purposes.
            std::string messageToString(const Headers& headers, const Body<T>& body) const override {
                return std::format("{}\r\n\r\n{}", headers.toString(), body.toString());
            }

            std::string name() const override { return http1_1_n::PROTOCOL_VERSION_NAME; }

            // Used to send request/response.
            std::vector<std::string> packetize(const Headers& headers, const Body<T>& body) const override {
                std::vector<std::string> packets { headers.build() };
                const std::vector<std::string>& bodyPackets = body.build(headers);
                packets.insert(packets.end(), bodyPackets.begin(), bodyPackets.end());
                return packets;
            }

            // Used when receiving request/response.
            std::tuple<startLineInformation_t, Headers, Body<T>> parse(const std::string& raw) const override {
                const auto& [rawHeaders, rawBody] = splitMessage(raw);
                Headers headers = Headers(headersParser());
                headers.parse(rawHeaders);
                Body<T> body = Body<T>(bodyParser());
                body.parse(headers, rawBody);
                return  { headersParser()->parseStartLine(headers.startLine()), headers, body };
            }

        private:
            const uint16_t DEFAULT_PORT = 80;

            std::pair<std::string, std::string> splitMessage(const std::string& message) const {
                const std::string HEADER_END_TOKEN = "\r\n\r\n";
                const size_t END_OF_HEADERS = message.find(HEADER_END_TOKEN);

                if (END_OF_HEADERS == std::string::npos)
                    throw InvalidArgument("Ill-formed", "HTTP message");

                // Returned pair = { Headers (string), Body (string) }.
                return std::make_pair(message.substr(0, END_OF_HEADERS), message.substr(END_OF_HEADERS  + HEADER_END_TOKEN.size()));
            }
        };
    }
}