#pragma once

#include <core/exception.h>
#include <core/network/detail/protocol/http1_1/body_parser.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/interface/protocol.h>
#include <format>
#include <memory>
#include <string>
#include <utility>


namespace network_n
{
    namespace protocol_n
    {
        template<typename T>
        class HTTP1_1 : public Protocol_i<T> {
        public:
            HTTP1_1() {}

            friend std::unique_ptr<HTTP1_1> std::make_unique<HTTP1_1>();

            // Used to send request/response.
            std::string build(const Headers& headers, const Body<T>& body) const override {
                return std::format("{}\r\n\r\n{}", headers.build(), body.build(headers));
            }

            std::unique_ptr<BodyParser_i<T>> bodyParser() const override { return std::make_unique<http1_1_n::BodyParser<T>>(); }

            uint16_t defaultPort() const override { return DEFAULT_PORT; }

            std::unique_ptr<HeadersParser_i> headersParser() const override { return std::make_unique<http1_1_n::HeadersParser>(); }

            std::string name() const override { return http1_1_n::PROTOCOL_VERSION_NAME; }

            // Used when receiving request/response.
            std::pair<Headers, Body<T>> parse(const std::string& raw) const override {
                const auto& [rawHeaders, rawBody] = splitMessage(raw);
                Headers headers = Headers(headersParser());
                headers.parse(rawHeaders);
                Body<T> body = Body<T>(bodyParser());
                body.parse(headers, rawBody);
                return std::make_pair(headers, body);
            }

            // Used when receiving request/response.
            std::unordered_map<std::string, std::string> parseStartLine(const std::string& startLine) const override { return http1_1_n::HeadersParser::parseStartLine(startLine); }

            // Used for debugging + tests purposes.
            std::string messageToString(const Headers& headers, const Body<T>& body) const override {
                return std::format("{}\r\n\r\n{}", headers.toString(), body.toString());
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