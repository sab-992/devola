#include <gtest/gtest.h>

#include <core/http/request.h>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <format>
#include <string>


using http_n::Request;

class RequestTest : public ::testing::Test {
protected:
    template<typename T>
    Request<T> createDummyRequest() {
        return std::move(Request<T>().setMethod("GET")
                                     .setAPIEndpoint("/")
                                     .setURL("www.test.com"));
    }
};

TEST_F(RequestTest, Constructor_HasDefaultProtocol) {
    using pugi::xml_document;
    Request<xml_document> request = Request<xml_document>();
    EXPECT_EQ(http_n::DEFAULT_PROTOCOL, request.protocol());
}

TEST_F(RequestTest, BuildWithURL_AddsHostHeader) {
    Request<std::string> request = createDummyRequest<std::string>();

    const std::string EXPECTED_URL = "www.test.com";
    const uint16_t EXPECTED_PORT = 4992;

    request.setURL(EXPECTED_URL);
    request.setPort(EXPECTED_PORT);
    request.build();

    const std::string EXPECTED_FULL_HOST_URL = std::format("{}:{}", EXPECTED_URL, EXPECTED_PORT);

    EXPECT_EQ(EXPECTED_FULL_HOST_URL, request.header("Host"));
}

TEST_F(RequestTest, BuildWithMethodAndAPIEndpoint_CreatesValidStartline) {
    using nlohmann::json;
    using network_n::protocol_n::Protocol;

    const std::string EXPECTED_METHOD = "POST";
    const std::string EXPECTED_API_ENDPOINT = "/test";
    const std::string EXPECTED_STARTLINE = std::format("{} {} HTTP/1.1", EXPECTED_METHOD, EXPECTED_API_ENDPOINT);

    Request<json> request = Request<json>().setMethod(EXPECTED_METHOD)
                                           .setAPIEndpoint(EXPECTED_API_ENDPOINT)
                                           .setProtocol(Protocol::HTTP1_1)
                                           .setURL("www.test.com").build();

    const std::string stringRequest = request.toString();
    std::string startLine = stringRequest.substr(0, stringRequest.find("\r\n"));

    EXPECT_EQ(EXPECTED_METHOD, request.method());
    EXPECT_EQ(EXPECTED_API_ENDPOINT, request.APIEndpoint());
    EXPECT_EQ(EXPECTED_STARTLINE, startLine);
}

TEST_F(RequestTest, BuildWithoutAPIEndpoint_ThrowsException) {
    Request<std::string> request = createDummyRequest<std::string>();
    request.setAPIEndpoint("");
    EXPECT_THROW(request.build(), std::invalid_argument);
}

TEST_F(RequestTest, BuildWithoutMethod_ThrowsException) {
    Request<std::string> request = createDummyRequest<std::string>();
    request.setMethod("");
    EXPECT_THROW(request.build(), std::invalid_argument);
}

TEST_F(RequestTest, BuildWithoutURL_ThrowsException) {
    Request<std::string> request = createDummyRequest<std::string>();
    request.setURL("");
    EXPECT_THROW(request.build(), std::invalid_argument);
}

// TODO: Test the Request::set(...) method. 