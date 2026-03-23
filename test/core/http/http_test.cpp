#include <gtest/gtest.h>
#include <core/http/utils/http_fake_server.h>
#include <core/http.h>
#include <string>
#include <format>

class HttpTest : public ::testing::TestWithParam<std::pair<std::string, std::string>> {
public:
    static void SetUpTestSuite() {
        m_httpFakeServer = HttpFakeServer::instance();
        m_httpFakeServer->run();
    }

    static void TearDownTestSuite() {
        m_httpFakeServer->stop();
    }

protected:
    static inline std::shared_ptr<HttpFakeServer> m_httpFakeServer = nullptr;
};

TEST_P(HttpTest, Get) {
    // auto [input, expected] = GetParam();

    // using http_n::Http;
    // std::shared_ptr<Http> http = Http::instance();
    
    // auto req =  Http::RequestPresets<std::string>::Get().setAPIEndpoint("/")
    //                                                     .setURL("127.0.0.1")
    //                                                     .setPort(m_httpFakeServer->port()).build();

    // auto res = http->sync->receive<std::string>(http->sync->send(req));
    // std::cout << "Response:\n" << res << std::endl;

    using namespace http_n;
    using namespace asio;

    auto [input, expected] = GetParam();
    const std::string message = "Bruh!";
    const std::string request = "GET / HTTP/1.1\r\nHost: 127.0.0.1:4000\r\nConnection: close\r\n" + std::format("Content-Length: {}\r\n\r\n{}", message.size(), message);
    

    io_context io_ctx;
    ip::tcp::resolver resolver(io_ctx);
    ip::tcp::socket socket(io_ctx);

    auto req = Request<std::string>().set(request).build();

    connect(socket, resolver.resolve("127.0.0.1", std::format("{}", m_httpFakeServer->port())));
    write(socket, buffer(req.get()));

    error_code ec;
    std::string response;
    read(socket, dynamic_buffer(response), transfer_all(), ec);

    if (ec and ec != error::eof)
        std::cout << "ERROR:\n" << ec.message() << std::endl;

    auto res = Response<std::string>().set(response).build();

    std::cout << "Response:\n" << res << std::endl;
}

INSTANTIATE_TEST_SUITE_P(HttpRequests, HttpTest, ::testing::Values(std::make_pair("T", "T")));