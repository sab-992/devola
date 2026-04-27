#include <gtest/gtest.h>

#include <core/exception.h>
#include <core/http/request.h>
#include <core/network/protocol_factory.h>
#include <core/xml/xml.h>
#include <nlohmann/json.hpp>
#include <format>
#include <string>
#include <utils/core/build_headers.h>


using http_n::Request;

class RequestTest : public ::testing::Test {};

TEST_F(RequestTest, Constructor_HasDefaultProtocol) {
    Request<xml_n::Document> request = Request<xml_n::Document>();
    EXPECT_EQ(http_n::DEFAULT_PROTOCOL, request.protocol());
}

TEST_F(RequestTest, BuildWithURL_AddsHostHeader) {
    const std::string EXPECTED_URL = "www.test.com";
    const uint16_t EXPECTED_PORT = 4992;
    const std::string EXPECTED_FULL_HOST_URL = std::format("{}:{}", EXPECTED_URL, EXPECTED_PORT);

    Request<std::string> request = Request<std::string>().setMethod("GET")
                                                         .setAPIEndpoint("/");

    request.setURL(EXPECTED_URL);
    request.setPort(EXPECTED_PORT);
    request.build();

    EXPECT_EQ(EXPECTED_FULL_HOST_URL, request.header("Host"));
}

TEST_F(RequestTest, BuildWithMethodAndAPIEndpoint_CreatesValidStartline) {
    using nlohmann::json;
    using namespace network_n::protocol_n;

    const std::string EXPECTED_METHOD = "POST";
    const std::string EXPECTED_API_ENDPOINT = "/test";
    const Protocol EXPECTED_PROTOCOL = Protocol::HTTP1_1;
    const std::string EXPECTED_STARTLINE = std::format("{} {} {}", EXPECTED_METHOD,
                                                                   EXPECTED_API_ENDPOINT,
                                                                   Factory::create<json>(EXPECTED_PROTOCOL)->name());

    Request<json> request = Request<json>().setMethod(EXPECTED_METHOD)
                                           .setAPIEndpoint(EXPECTED_API_ENDPOINT)
                                           .setProtocol(EXPECTED_PROTOCOL)
                                           .setURL("www.test.com").build();

    const std::string stringRequest = request.toString();
    std::string startLine = stringRequest.substr(0, stringRequest.find("\r\n"));

    EXPECT_EQ(EXPECTED_METHOD, request.method());
    EXPECT_EQ(EXPECTED_API_ENDPOINT, request.APIEndpoint());
    EXPECT_EQ(EXPECTED_PROTOCOL, request.protocol());
    EXPECT_EQ(EXPECTED_STARTLINE, startLine);
}

TEST_F(RequestTest, BuildWithoutAPIEndpoint_ThrowsException) {
    Request<std::string> request = Request<std::string>().setMethod("GET")
                                                         .setURL("www.test.com");
    EXPECT_THROW(request.build(), InvalidArgument);
}

TEST_F(RequestTest, BuildWithoutMethod_ThrowsException) {
    Request<std::string> request = Request<std::string>().setAPIEndpoint("/")
                                                         .setURL("www.test.com");
    EXPECT_THROW(request.build(), InvalidArgument);
}

TEST_F(RequestTest, BuildWithoutURL_ThrowsException) {
    Request<std::string> request = Request<std::string>().setMethod("GET")
                                                         .setAPIEndpoint("/");
    EXPECT_THROW(request.build(), InvalidArgument);
}

TEST_F(RequestTest, BuildWithoutProtocol_ThrowsException) {
    using network_n::protocol_n::Protocol; 
    Request<std::string> request = Request<std::string>().setMethod("GET")
                                                         .setAPIEndpoint("/")
                                                         .setURL("www.test.com");

    request.setProtocol(Protocol::NONE);

    EXPECT_THROW(request.build(), InvalidArgument);
}

TEST_F(RequestTest, Set_CreatesValidHTTPRequest) {
    using namespace http_n;
    using namespace network_n;

    const std::string EXPECTED_METHOD = "DELETE";
    const std::string EXPECTED_API_ENDPOINT = "/resource/1";
    const protocol_n::Protocol EXPECTED_PROTOCOL = protocol_n::Protocol::HTTP1_1;
    const std::string EXPECTED_URL = "www.test2.com";
    const uint16_t EXPECTED_PORT = 5503;
    const std::string EXPECTED_BODY = "Hello world!";
    const std::unordered_map<std::string, std::string> headersUMap{ {"Host",              std::format("{}:{}", EXPECTED_URL, EXPECTED_PORT) },
                                                                    {"Transfer-encoding", "chunked" },
                                                                    {"Accept",            "application/xml" },
                                                                    {"Content-Length",    std::to_string(EXPECTED_BODY.size()) } };

    const std::string EXPECTED_REQUEST = std::format("{} {} {}\r\n{}\r\n\r\n{}", EXPECTED_METHOD,
                                                                                 EXPECTED_API_ENDPOINT,
                                                                                 protocol_n::Factory::create<std::string>(EXPECTED_PROTOCOL)->name(),
                                                                                 buildHeaders(headersUMap),
                                                                                 EXPECTED_BODY);

    Request<std::string> request = Request<std::string>().set(EXPECTED_REQUEST);

    EXPECT_NO_THROW(request.build());

    EXPECT_EQ(EXPECTED_METHOD, request.method());
    EXPECT_EQ(EXPECTED_API_ENDPOINT, request.APIEndpoint());
    EXPECT_EQ(EXPECTED_PROTOCOL, request.protocol());
    EXPECT_EQ(EXPECTED_URL, request.url());
    EXPECT_EQ(EXPECTED_PORT, request.port());
    EXPECT_EQ(headersUMap.size(), request.headersMap().size());
    for (const auto& [header, expected_value]: headersUMap)
        EXPECT_EQ(expected_value, request.header(header));
    EXPECT_EQ(EXPECTED_BODY, request.body());
}