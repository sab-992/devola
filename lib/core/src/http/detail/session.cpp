#include <core/http/detail/session.hpp>


http_n::server_n::Session::Session(tcp::socket&& socket) : m_socket(std::move(socket)) {}

http_n::server_n::Session::~Session() {}

asio::awaitable<std::string> http_n::server_n::Session::read() {
    std::string data;
    std::size_t n_bytes = co_await async_read_until(m_socket, asio::dynamic_buffer(data), "\r\n\r\n", asio::use_awaitable);
    m_light->log(log_n::Level_en::INFO, "Received", n_bytes, "bytes from", std::format("[{}].", Converter::toString(m_socket.remote_endpoint())));
    co_return data;
}

asio::awaitable<void> http_n::server_n::Session::write(const http_n::Response<std::string>& response) {
    const std::string stringResponse = response.toString();
    const size_t bytesWritten = stringResponse.size();

    co_await asio::async_write(m_socket, asio::buffer(stringResponse, bytesWritten), asio::use_awaitable);
    m_light->log(log_n::Level_en::INFO, "Sent", bytesWritten, "bytes to", std::format("[{}].", Converter::toString(m_socket.remote_endpoint())));
}

asio::awaitable<void> http_n::server_n::Session::error(network_n::Code errorCode) {
    http_n::Response<std::string> response;
    response.setStatus(errorCode).build();
    co_await write(response);
}