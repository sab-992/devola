#include <core/http/http.hpp>


http_n::Http::Http(asio::io_context* ctx) : m_ioContext(ctx) {}

#ifdef DEBUG_MODE_ENABLED
    void http_n::Http::disablePeerVerification() {
        m_sslMode = asio::ssl::verify_none;
    }
#endif

asio::awaitable<http_n::Response> http_n::Http::async_receive(sslSocket_t& socket) const {
    const std::string& alpnExtension = readALPNExtension(socket);

    auto version = network_n::version_n::Factory::create(alpnExtension);

    const auto& [headers, body] = co_await version->async_receive(socket);

    co_return Response(headers, body);
}

asio::awaitable<http_n::Response> http_n::Http::async_receive(sslSocket_t&& socket) const {
    return async_receive(socket);
}

asio::awaitable<http_n::sslSocket_t> http_n::Http::async_send(const Request& request) const {
    if (not m_ioContext)
        throw Exception("m_ioContext is nullptr");

    using namespace asio;
    using namespace asio::ip;

    auto version = request.version();

    sslSocket_t socket = prepareSSLHandshake(version, request.url());

    tcp::resolver resolver(*m_ioContext);
    co_await async_connect(socket.lowest_layer(), co_await resolver.async_resolve(request.url(), std::to_string(request.port())));

    co_await socket.async_handshake(asio::ssl::stream_base::client);

    co_await request.version()->async_send(socket, request.toString());

    co_return std::move(socket);
}

void http_n::Http::negotiateAlpnExtension(asio::ssl::context& context, std::string_view extension) const {
    const std::string& prefixedProtos = std::format("{}{}", static_cast<char>(extension.size()), extension);
    const unsigned char* protos = reinterpret_cast<const unsigned char*>(prefixedProtos.data());
    SSL_CTX_set_alpn_protos(context.native_handle(), protos, prefixedProtos.size());
}

http_n::sslSocket_t http_n::Http::prepareSSLHandshake(std::shared_ptr<network_n::version_n::Version_i> version, const std::string& hostName) const {
    if (not m_ioContext)
        throw Exception("m_ioContext is nullptr");

    namespace ssl = asio::ssl;
    using namespace asio::ip;

    ssl::context sslContext(ssl::context::tls_client);

    negotiateAlpnExtension(sslContext, version->alpnExtension());

    sslContext.set_verify_mode(m_sslMode);
    sslContext.set_default_verify_paths();

    asio::ssl::stream<tcp::socket> socket(*m_ioContext, sslContext);
    SSL_set_tlsext_host_name(socket.native_handle(), hostName.c_str());

    return std::move(socket);
}

std::string http_n::Http::readALPNExtension(sslSocket_t& socket) {
    const unsigned char* alpn;
    unsigned int alpn_len;

    SSL* ssl = socket.native_handle();
    SSL_get0_alpn_selected(ssl, &alpn, &alpn_len);

    if (not alpn or alpn_len <= 0)
        return "http/1.1"; // Default protocol.

    return std::string(reinterpret_cast<const char*>(alpn), alpn_len);
}

http_n::Response http_n::Http::receive(sslSocket_t& socket) const {
    const std::string& alpnExtension = readALPNExtension(socket);

    auto version = network_n::version_n::Factory::create(alpnExtension);

    const auto& [headers, body] = version->receive(socket);

    return Response(headers, body);
}

http_n::Response http_n::Http::receive(sslSocket_t&& socket) const { return receive(socket); }

http_n::sslSocket_t http_n::Http::send(const Request& request) const {
    if (not m_ioContext)
        throw Exception("m_ioContext is nullptr");

    using namespace asio;
    using namespace asio::ip;

    auto version = request.version();

    sslSocket_t socket = prepareSSLHandshake(version, request.url());

    tcp::resolver resolver(*m_ioContext);
    connect(socket.lowest_layer(), resolver.resolve(request.url(), std::to_string(request.port())));

    socket.handshake(asio::ssl::stream_base::client);

    version->send(socket, request.toString());

    return std::move(socket);
}