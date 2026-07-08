#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <core/exception.hpp>
#include <core/network/detail/protocol/http1_1/body_parser.hpp>
#include <core/network/detail/protocol/http1_1/headers_parser.hpp>
#include <core/network/interface/protocol.hpp>
#include <core/network/network.hpp>
#include <core/utility/singleton.hpp>
#include <format>
#include <memory>
#include <string>
#include <tuple>


namespace network_n
{
    namespace protocol_n
    {
        class Http1_1 : public Protocol_i, public Singleton<Http1_1> {
        public:
            Http1_1(const Singleton<Http1_1>::Private_s&);
            ~Http1_1() = default;

            std::string alpnExtension() const override;

            asio::awaitable<std::pair<Headers, Body>> async_receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const override;
            asio::awaitable<void> async_send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const override;

            std::shared_ptr<BodyParser_i> bodyParser() const override;

            uint16_t defaultPort() const override;

            std::shared_ptr<HeadersParser_i> headersParser() const override;

            // Used for logging, and tests purposes.
            std::string messageToString(const Headers& headers, const Body& body) const override;

            std::string name() const override;

            // Used to send request/response.
            std::vector<std::string> packetize(const Headers& headers, const Body& body) const override;
            // Used when receiving request/response.
            std::tuple<startLineInformation_t, Headers, Body> parse(std::string_view raw) const override;
            std::tuple<startLineInformation_t, Headers, Body> parse(std::string_view rawHeaders, std::string_view rawBody) const override;

            std::pair<Headers, Body> receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const override;
            void send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const override;

        private:
            const std::string END_OF_HEADERS_TOKEN = "\r\n\r\n";
            const uint16_t DEFAULT_PORT = 443;

            std::pair<std::string_view, std::string_view> splitMessage(std::string_view message) const;
        };
    }
}