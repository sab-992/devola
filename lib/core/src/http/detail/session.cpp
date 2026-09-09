#include <core/http/detail/session.hpp>


http_n::server_n::Session::Session(const Private&, http_n::sslSocket_t&& socket) : m_isClosing(false), m_socket(std::move(socket)), m_sslEstablished(false) {
    m_remoteEndpoint = Converter::toString(m_socket.next_layer().remote_endpoint());
}

http_n::server_n::Session::~Session() {}

std::string http_n::server_n::Session::alpnExtension() {
    return http_n::Http::readALPNExtension(m_socket);
}

std::shared_ptr<http_n::server_n::Session> http_n::server_n::Session::create(sslSocket_t&& socket) {
    return std::make_shared<Session>(Private(), std::move(socket));
}

void http_n::server_n::Session::error(network_n::Code errorCode) {
    http_n::Response response;
    response.setStatus(errorCode).build();
    write(response);
}

asio::awaitable<void> http_n::server_n::Session::handshake() {
    co_await m_socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);
    m_sslEstablished = true;
}

void http_n::server_n::Session::onWriteCompleted(std::shared_ptr<std::string> stringResponse, const std::error_code& ec, std::size_t size) {
    if (not ec and size != stringResponse->size())
        return;
    else if (ec)
        m_light->log(log_n::Level_en::ERROR, FUNCTION_SIGNATURE, "Error during socket write:", ec);

    shutdown();
}

asio::awaitable<http_n::Request> http_n::server_n::Session::read() {
    validateSSLContext();

    std::shared_ptr<network_n::version_n::Version_i> version = network_n::version_n::Factory::create(alpnExtension());
    const auto& [headers, body] = co_await version->async_receive(m_socket);
    Request request(headers, body);
    request.setProtocol(version);
    m_light->log(log_n::Level_en::DEBUG, "Received", request.toString().size(), "bytes from", std::format("[{}].", m_remoteEndpoint));
    co_return request;
}

void http_n::server_n::Session::shutdown() {
    if (m_isClosing) return;
    m_isClosing = true;
    m_socket.async_shutdown([self = shared_from_this()](const asio::error_code& ec){ if (self->m_socket.lowest_layer().is_open()) self->m_socket.lowest_layer().close(); });
}

void http_n::server_n::Session::validateSSLContext() const {
    if (not m_sslEstablished)
        throw Exception("SSL context was not established.");
}

void http_n::server_n::Session::write(http_n::Response& response) {
    validateSSLContext();

    const std::string CHUNKED = "chunked";
    const std::string CONTENT_LENGTH = "Content-Length";
    const std::string TRANSFER_ENCODING = "Transfer-Encoding";
    const std::string RESPONSE_BODY_SIZE = std::to_string(response.body<std::string>().size());

    if (response.header(TRANSFER_ENCODING) != CHUNKED and response.header(CONTENT_LENGTH) != RESPONSE_BODY_SIZE)
        response.setHeader(CONTENT_LENGTH, RESPONSE_BODY_SIZE);

    auto stringResponse = std::make_shared<std::string>(response.toString());
    asio::async_write(m_socket, asio::buffer(*stringResponse), std::bind_front(&Session::onWriteCompleted, shared_from_this(), stringResponse));
    m_light->log(log_n::Level_en::DEBUG, "Sent", stringResponse->size(), "bytes to", std::format("[{}].", m_remoteEndpoint));
}

void http_n::server_n::Session::write(http_n::Response&& response) {
    write(response);
}