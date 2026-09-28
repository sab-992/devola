#include <gtest/gtest.h>

#include <core/exception.hpp>
#include <core/http/request.hpp>
#include <core/network/detail/version/factory.hpp>
#include <core/xml/document.hpp>
#include <nlohmann/json.hpp>
#include <format>
#include <string>
#include <helper/core/build_headers.hpp>
#include <helper/core/inner_types.hpp>


using http_n::Request;

template<typename T>
class RequestTest : public ::testing::Test {
protected:
    auto getTestObject(bool alt=false) {
        return InnerTypes<T>::getTestObject(InnerTypes<T>::getTestString(alt));
    }

    std::string getTestStringObject(bool alt=false) {
        return InnerTypes<T>::getTestStringFromObject(this->getTestObject(alt));
    }
};

TYPED_TEST_SUITE(RequestTest, innerTypes_t);

TYPED_TEST(RequestTest, Constructor_HasDefaultProtocol) {
    Request request;
    EXPECT_EQ(network_n::version_n::Factory::create(http_n::DEFAULT_PROTOCOL), request.version());
}

TYPED_TEST(RequestTest, BuildWithURL_AddsHostHeader) {
    const std::string EXPECTED_URL = "www.test.com";
    const uint16_t EXPECTED_PORT = 4992;
    const std::string EXPECTED_FULL_HOST_URL = std::format("{}:{}", EXPECTED_URL, EXPECTED_PORT);
    Request request;
    request.setMethod("GET")
           .setAPIEndpoint("/");

    request.setURL(EXPECTED_URL)
           .setPort(EXPECTED_PORT)
           .build();

    EXPECT_EQ(EXPECTED_FULL_HOST_URL, request.header("Host"));
}

TYPED_TEST(RequestTest, BuildWithMethodAndAPIEndpoint_CreatesValidStartline) {
    using nlohmann::json;
    using namespace http_n::version_n;
    using namespace network_n::version_n;

    const std::string EXPECTED_METHOD = "POST";
    const std::string EXPECTED_API_ENDPOINT = "/test";
    const Version EXPECTED_PROTOCOL = Version::HTTP1_1;
    const auto version = Factory::create(EXPECTED_PROTOCOL);
    const std::string EXPECTED_STARTLINE = std::format("{} {} {}", EXPECTED_METHOD,
                                                                   EXPECTED_API_ENDPOINT,
                                                                   version->name());
    Request request;
    request.setMethod(EXPECTED_METHOD)
           .setAPIEndpoint(EXPECTED_API_ENDPOINT)
           .setProtocol(EXPECTED_PROTOCOL)
           .setURL("www.test.com").build();

    const std::string stringRequest = request.toString();
    const std::string startLine = stringRequest.substr(0, stringRequest.find("\r\n"));

    EXPECT_EQ(EXPECTED_STARTLINE, startLine);
}

TYPED_TEST(RequestTest, PrepareTransmissionPackets_ReturnsPacketsToSend) {
    using namespace http_n;
    using namespace network_n;
    using network_n::Code;

    const std::vector<std::string> EXPECTED_PACKETS = { "POST / HTTP/1.1\r\nHost: www.test.com:443", this->getTestStringObject() };
    Request request;
    request.setMethod("POST")
           .setAPIEndpoint("/")
           .setProtocol(http_n::DEFAULT_PROTOCOL)
           .setURL("www.test.com")
           .setBody<TypeParam>(this->getTestObject()).build();

    const auto result = request.prepareTransmissionPackets();

    EXPECT_EQ(EXPECTED_PACKETS, result);
}

TYPED_TEST(RequestTest, BuildWithoutAPIEndpoint_ThrowsException) {
    Request request;

    request.setMethod("GET")
           .setURL("www.test.com");

    EXPECT_THROW(request.build(), InvalidArgument);
}

TYPED_TEST(RequestTest, BuildWithoutMethod_ThrowsException) {
    Request request;

    request.setAPIEndpoint("/")
           .setURL("www.test.com");

    EXPECT_THROW(request.build(), InvalidArgument);
}

TYPED_TEST(RequestTest, BuildWithoutURL_ThrowsException) {
    Request request;

    request.setMethod("GET")
           .setAPIEndpoint("/");

    EXPECT_THROW(request.build(), InvalidArgument);
}

TYPED_TEST(RequestTest, Set_CreatesValidHTTPRequest) {
    using namespace http_n;
    using namespace network_n;

    const std::string EXPECTED_METHOD = "DELETE";
    const std::string EXPECTED_API_ENDPOINT = "/resource/1";
    const std::string EXPECTED_URL = "www.test2.com";
    const uint16_t EXPECTED_PORT = 5503;
    const auto EXPECTED_BODY = this->getTestObject();
    const auto EXPECTED_STRING_BODY = this->getTestStringObject();
    const auto EXPECTED_PROTOCOL = network_n::version_n::Factory::create(http_n::DEFAULT_PROTOCOL);
    const std::unordered_map<std::string, std::string> headersUMap{ { "Host",              std::format("{}:{}", EXPECTED_URL, EXPECTED_PORT) },
                                                                    { "Transfer-encoding", "chunked" },
                                                                    { "Accept",            "application/xml" },
                                                                    { "Content-Length",    std::to_string(EXPECTED_STRING_BODY.size()) } };
    const std::string EXPECTED_REQUEST = std::format("{} {} {}\r\n{}\r\n\r\n{}", EXPECTED_METHOD,
                                                                                 EXPECTED_API_ENDPOINT,
                                                                                 EXPECTED_PROTOCOL->name(),
                                                                                 buildHeaders(headersUMap),
                                                                                 EXPECTED_STRING_BODY);
    Request request;

    request.set(EXPECTED_REQUEST);

    EXPECT_NO_THROW(request.build());
    EXPECT_EQ(EXPECTED_METHOD, request.method());
    EXPECT_EQ(EXPECTED_API_ENDPOINT, request.APIEndpoint());
    EXPECT_EQ(EXPECTED_PROTOCOL, request.version());
    EXPECT_EQ(EXPECTED_URL, request.url());
    EXPECT_EQ(EXPECTED_PORT, request.port());
    EXPECT_EQ(headersUMap.size(), request.headersMap().size());
    for (const auto& [header, expected_value]: headersUMap)
        EXPECT_EQ(expected_value, request.header(header));
    EXPECT_EQ(EXPECTED_BODY, request.body<TypeParam>());
}