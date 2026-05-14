#pragma once

#include <asio.hpp>
#include <core/exception.h>
#include <core/network/detail/protocol/http1_1/body_parser.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/interface/protocol.h>
#include <core/network/network.h>
#include <core/utility/singleton.h>
#include <format>
#include <memory>
#include <string>
#include <tuple>


namespace network_n
{
    namespace protocol_n
    {
        template<typename T>
        class Http1_1 : public Protocol_i<T>, public Singleton<Http1_1<T>> {
        public:
            Http1_1(const Singleton<Http1_1<T>>::Creator_s&) {}

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
                return parse(rawHeaders, rawBody);
            }

            std::tuple<startLineInformation_t, Headers, Body<T>> parse(const std::string& rawHeaders, const std::string& rawBody) const override {
                Headers headers = Headers(headersParser());
                headers.parse(rawHeaders);
                Body<T> body = Body<T>(bodyParser());
                body.parse(headers, rawBody);
                return  { headersParser()->parseStartLine(headers.startLine()), headers, body };
            }

            std::string async_receive() const override { /* TODO */ return ""; }

            std::pair<Headers, Body<T>> receive(asio::ip::tcp::socket& socket) const override {
                using namespace network_n;
                using namespace asio;

                const std::string END_OF_HEADERS_TOKEN = "\r\n\r\n";

                std::string rawHeaders;
                read_until(socket,  dynamic_buffer(rawHeaders), std::string_view(END_OF_HEADERS_TOKEN));

                size_t endOfHeadersPos = rawHeaders.find(END_OF_HEADERS_TOKEN);
                const std::string extractedPartOfBody = rawHeaders.substr(endOfHeadersPos + END_OF_HEADERS_TOKEN.size());

                std::string rawBody;
                auto parser = headersParser();
                Headers headers(parser);
                headers.parse(rawHeaders.substr(0, endOfHeadersPos));
                if (http1_1_n::HeadersParser::isContentChunked(headers))
                    read_until(socket,  dynamic_buffer(rawBody), std::string_view(std::format("0{}", END_OF_HEADERS_TOKEN)));
                else if (not headers.get("Content-Length").empty()) {
                    unsigned long contentLength = std::stoul(headers.get("Content-Length"));
                    read(socket, dynamic_buffer(rawBody), transfer_exactly(contentLength - extractedPartOfBody.size()));
                }

                Body<T> body(bodyParser());
                body.parse(headers, extractedPartOfBody + rawBody);

                return { std::move(headers), std::move(body) };
            }

            void async_send() const override { /* TODO */ }
            void send() const override { /* TODO */ }

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