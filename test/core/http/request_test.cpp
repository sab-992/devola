#include <gtest/gtest.h>

#include <core/http/request.h>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <format>
#include <string>


using http_n::Request;

class RequestTests : public ::testing::Test {
protected:
    template<typename T>
    Request<T> createDummyRequest() {
        return std::move(Request<T>().setMethod("GET")
                                     .setAPIEndpoint("/")
                                     .setURL("www.test.com"));
    }
};

TEST_F(RequestTests, Constructor_HasDefaultProtocol) {
    using pugi::xml_document;
    Request<xml_document> request = Request<xml_document>();
    EXPECT_EQ(http_n::DEFAULT_PROTOCOL, request.protocol());
}

TEST_F(RequestTests, SetHeader_AddsNewHeader) {
    Request<std::string> request = Request<std::string>();

    const std::string EXPECTED_HEADER_NAME = "Transfer-encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";

    request.setHeader(EXPECTED_HEADER_NAME, EXPECTED_HEADER_VALUE);

    EXPECT_EQ(EXPECTED_HEADER_VALUE, request.header(EXPECTED_HEADER_NAME));
}

TEST_F(RequestTests, SetHeader_IncrementsSize) {
    Request<std::string> request = Request<std::string>();

    const size_t EXPECTED_MAP_SIZE_INCREASE = 1;
    const size_t EXPECTED_HEADERS_MAP_SIZE = 3;

    request.setHeader("Accept", "text/html");
    request.setHeader("Content-Length", "8");

    const size_t PREVIOUS_MAP_SIZE = request.headersMap().size();
    request.setHeader("Transfer-encoding", "chunked");

    EXPECT_EQ(EXPECTED_MAP_SIZE_INCREASE, request.headersMap().size() - PREVIOUS_MAP_SIZE);
    EXPECT_EQ(EXPECTED_HEADERS_MAP_SIZE, request.headersMap().size());
}

TEST_F(RequestTests, SetHeader_OverwritesExistingHeader) {
    Request<std::string> request = Request<std::string>();

    const std::string EXPECTED_HEADER_NAME = "Accept";
    const std::string EXPECTED_HEADER_VALUE = "text/html";
    const size_t EXPECTED_HEADERS_MAP_SIZE = 1;

    request.setHeader(EXPECTED_HEADER_NAME, "application/json");
    request.setHeader(EXPECTED_HEADER_NAME, EXPECTED_HEADER_VALUE);

    EXPECT_EQ(EXPECTED_HEADERS_MAP_SIZE, request.headersMap().size());
    EXPECT_EQ(EXPECTED_HEADER_VALUE, request.header(EXPECTED_HEADER_NAME));
}

TEST_F(RequestTests, BuildWithURL_AddsHostHeader) {
    Request<std::string> request = createDummyRequest<std::string>();

    const std::string EXPECTED_URL = "www.test.com";
    const uint16_t EXPECTED_PORT = 4992;

    request.setURL(EXPECTED_URL);
    request.setPort(EXPECTED_PORT);
    request.build();

    const std::string EXPECTED_FULL_HOST_URL = std::format("{}:{}", EXPECTED_URL, EXPECTED_PORT);

    EXPECT_EQ(EXPECTED_FULL_HOST_URL, request.header("Host"));
}

TEST_F(RequestTests, BuildWithMethodAndAPIEndpoint_CreatesValidStartline) {
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

TEST_F(RequestTests, BuildWithoutAPIEndpoint_ThrowsException) {
    Request<std::string> request = createDummyRequest<std::string>();
    request.setAPIEndpoint("");
    EXPECT_THROW(request.build(), std::invalid_argument);
}

TEST_F(RequestTests, BuildWithoutMethod_ThrowsException) {
    Request<std::string> request = createDummyRequest<std::string>();
    request.setMethod("");
    EXPECT_THROW(request.build(), std::invalid_argument);
}

TEST_F(RequestTests, BuildWithoutProtocol_ThrowsException) {
    using network_n::protocol_n::Protocol; 
    Request<std::string> request = createDummyRequest<std::string>();
    request.setProtocol(Protocol::NONE);
    EXPECT_THROW(request.build(), std::invalid_argument);
}

TEST_F(RequestTests, BuildWithoutURL_ThrowsException) {
    Request<std::string> request = createDummyRequest<std::string>();
    request.setURL("");
    EXPECT_THROW(request.build(), std::invalid_argument);
}

// TODO: Test the Request::set(...) method. 