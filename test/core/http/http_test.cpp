#include <gtest/gtest.h>
#include <core/http/utils/http_fake_server.h>
#include <core/http/http.h>
#include <string>
#include <format>

class HttpTest : public ::testing::TestWithParam<std::pair<std::string, std::string>> {
protected:
    static void SetUpTestSuite() {
        m_http_server = HttpFakeServer::getInstance();
        m_http_server->run();
    }

    static void TearDownTestSuite() {
        m_http_server->stop();
    }

    static inline std::shared_ptr<HttpFakeServer> m_http_server = nullptr;
};

TEST_P(HttpTest, Get) {
    auto [input, expected] = GetParam();

    // std::unique_ptr<http_n::Response_i<std::string>> Response = Http<std::string>::get("/", { "127.0.0.1", m_http_server->port() });
}

// TEST_P(HttpTest, Post) {
//     auto [input, expected] = GetParam();
//     std::string message = "Bruh!";
//     std::string request = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n" + std::format("Content-Length: {}\r\n\r\n{}", message.size(), message);
//     asio::io_context io_ctx;
//     asio::ip::tcp::resolver resolver(io_ctx);
//     asio::ip::tcp::socket socket(io_ctx);
//     asio::connect(socket, resolver.resolve("127.0.0.1", std::format("{}", m_http_server->port())));
//     asio::write(socket, asio::dynamic_buffer(request));
//     std::string response;
//     asio::error_code ec;
//     asio::read(socket, asio::dynamic_buffer(response), ec);
//     std::cout << "Response: " << response << std::endl;
// }

INSTANTIATE_TEST_SUITE_P(HttpRequests, HttpTest, ::testing::Values(std::make_pair("T", "T")));