#include <core/network/detail/protocol/http1_1/http1_1.hpp>


network_n::protocol_n::Http1_1::Http1_1(const Singleton<Http1_1>::Private_s&) {}

std::string network_n::protocol_n::Http1_1::alpnExtension() const {
    return "http/1.1";
};

asio::awaitable<std::pair<network_n::Headers, network_n::Body>> network_n::protocol_n::Http1_1::async_receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const {
    using namespace network_n;
    using namespace asio;

    std::string rawHeaders;
    co_await async_read_until(socket,  dynamic_buffer(rawHeaders), std::string_view(END_OF_HEADERS_TOKEN), use_awaitable);

    size_t endOfHeadersPos = rawHeaders.find(END_OF_HEADERS_TOKEN);
    const std::string extractedPartOfBody = rawHeaders.substr(endOfHeadersPos + END_OF_HEADERS_TOKEN.size());

    std::string rawBody;
    auto parser = headersParser();
    Headers headers(parser);
    headers.parse(rawHeaders.substr(0, endOfHeadersPos));
    if (http1_1_n::HeadersParser::isContentChunked(headers))
        co_await async_read_until(socket,  dynamic_buffer(rawBody), std::string_view(std::format("0{}", END_OF_HEADERS_TOKEN)), use_awaitable);
    else if (not headers.get("Content-Length").empty()) {
        unsigned long contentLength = std::stoul(headers.get("Content-Length"));
        co_await async_read(socket, dynamic_buffer(rawBody), transfer_exactly(contentLength - extractedPartOfBody.size()), use_awaitable);
    }

    Body body(bodyParser());
    body.parse(headers, extractedPartOfBody + rawBody);

    std::pair<network_n::Headers, network_n::Body> result = { std::move(headers), std::move(body)};
    co_return result;
}

asio::awaitable<void> network_n::protocol_n::Http1_1::async_send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const {
    using namespace asio;
    co_await async_write(socket, buffer(stringRequest));
}

std::shared_ptr<network_n::protocol_n::BodyParser_i> network_n::protocol_n::Http1_1::bodyParser() const {
    return http1_1_n::BodyParser::instance();
}

uint16_t network_n::protocol_n::Http1_1::defaultPort() const {
    return DEFAULT_PORT;
}

std::shared_ptr<network_n::protocol_n::HeadersParser_i> network_n::protocol_n::Http1_1::headersParser() const {
    return http1_1_n::HeadersParser::instance();
}

std::string network_n::protocol_n::Http1_1::messageToString(const Headers& headers, const Body& body) const {
    return std::format("{}\r\n\r\n{}", headers.toString(), body.toString());
}

std::string network_n::protocol_n::Http1_1::name() const {
    return http1_1_n::PROTOCOL_VERSION_NAME;
}

std::vector<std::string> network_n::protocol_n::Http1_1::packetize(const Headers& headers, const Body& body) const {
    std::vector<std::string> packets { headers.build() };
    const std::vector<std::string>& bodyPackets = body.build(headers);
    packets.insert(packets.end(), bodyPackets.begin(), bodyPackets.end());
    return packets;
}

std::tuple<startLineInformation_t, network_n::Headers, network_n::Body> network_n::protocol_n::Http1_1::parse(std::string_view raw) const {
    const auto& [rawHeaders, rawBody] = splitMessage(raw);
    return parse(rawHeaders, rawBody);
}

std::tuple<startLineInformation_t, network_n::Headers, network_n::Body> network_n::protocol_n::Http1_1::parse(std::string_view rawHeaders, std::string_view rawBody) const {
    Headers headers = Headers(headersParser());
    headers.parse(rawHeaders);
    Body body = Body(bodyParser());
    body.parse(headers, rawBody);
    return  { headersParser()->parseStartLine(headers.startLine()), headers, body };
}

std::pair<network_n::Headers, network_n::Body> network_n::protocol_n::Http1_1::receive(asio::ssl::stream<asio::ip::tcp::socket>& socket) const {
    using namespace network_n;
    using namespace asio;

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

    Body body(bodyParser());
    body.parse(headers, extractedPartOfBody + rawBody);

    return { std::move(headers), std::move(body) };
}

void network_n::protocol_n::Http1_1::send(asio::ssl::stream<asio::ip::tcp::socket>& socket, const std::string& stringRequest) const {
    using namespace asio;
    write(socket, buffer(stringRequest));
}

std::pair<std::string_view, std::string_view> network_n::protocol_n::Http1_1::splitMessage(std::string_view message) const {
    const size_t END_OF_HEADERS = message.find(END_OF_HEADERS_TOKEN);

    if (END_OF_HEADERS == std::string::npos)
        throw InvalidArgument("Ill-formed", "HTTP message");

    // Returned pair = { Headers (string), Body (string) }.
    return std::make_pair(message.substr(0, END_OF_HEADERS),
                            message.substr(END_OF_HEADERS  + END_OF_HEADERS_TOKEN.size()));
}