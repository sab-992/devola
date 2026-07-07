#include <core/http/detail/session.hpp>


http_n::server_n::Session::Session(http_n::sslSocket_t&& socket) : m_socket(std::move(socket)), m_sslEstablished(false) {
    m_remoteEndpoint = Converter::toString(m_socket.next_layer().remote_endpoint());
}

http_n::server_n::Session::~Session() {}

asio::awaitable<void> http_n::server_n::Session::error(network_n::Code errorCode) {
    http_n::Response<std::string> response;
    response.setStatus(errorCode).build();
    co_await write(response);
}

asio::awaitable<void> http_n::server_n::Session::handshake() {
    co_await m_socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);
    m_sslEstablished = true;
}

asio::awaitable<std::string> http_n::server_n::Session::read() {
    validateSSLContext();
    std::string data;
    std::size_t n_bytes = co_await async_read_until(m_socket, asio::dynamic_buffer(data), "\r\n\r\n", asio::use_awaitable);
    m_light->log(log_n::Level_en::INFO, "Received", n_bytes, "bytes from", std::format("[{}].", m_remoteEndpoint));
    co_return data;
}

std::string http_n::server_n::Session::alpnExtension() {
    return http_n::Http::readALPNExtension(m_socket);
}

void http_n::server_n::Session::validateSSLContext() const {
    if (not m_sslEstablished)
        throw Exception("SSL context was not established.");
}

asio::awaitable<void> http_n::server_n::Session::write(const http_n::Response<std::string>& response) {
    validateSSLContext();
    const std::string stringResponse = response.toString();
    const size_t bytesWritten = stringResponse.size();
    co_await asio::async_write(m_socket, asio::buffer(stringResponse, bytesWritten), asio::use_awaitable);
    m_light->log(log_n::Level_en::INFO, "Sent", bytesWritten, "bytes to", std::format("[{}].", m_remoteEndpoint));
}