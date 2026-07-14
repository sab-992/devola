#include <gtest/gtest.h>

#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/http/detail/protocol/http1_1/http1_1.hpp>
#include <helper/core/body_parser_mock.hpp>
#include <helper/core/headers_parser_mock.hpp>
#include <helper/core/inner_types.hpp>

using http_n::protocol_n::Http1_1;

template<typename T>
class Http1_1Test : public ::testing::Test {
protected:
    auto getTestObject(bool alt=false) {
        return InnerTypes<T>::getTestObject(InnerTypes<T>::getTestString(alt));
    }

    std::string getTestStringObject(bool alt=false) {
        return InnerTypes<T>::getTestStringFromObject(this->getTestObject(alt));
    }

    auto getBodyParserMock(bool alt=false) {
        return BodyParserMock::get(InnerTypes<T>::getTestString(alt));
    }
};

TYPED_TEST_SUITE(Http1_1Test, innerTypes_t);

TYPED_TEST(Http1_1Test, Packetize_ReturnsVectorContainingPacketsToSend) {
    using namespace network_n;

    auto headerParser = HeadersParserMock::get(); Headers headers(headerParser);
    auto bodyParser = this->getBodyParserMock(); Body body(bodyParser);

    const std::vector<std::string> EXPECTED_PACKETS = { headerParser->build(headers), bodyParser->build(headers, body)[0] };
    auto protocol = Http1_1::instance();

    const std::vector<std::string> result = protocol->packetize(headers, body);

    EXPECT_EQ(EXPECTED_PACKETS, result);
}

TYPED_TEST(Http1_1Test, Parse_ReturnsStartLineInformationAndHeadersAndBodyObjectTuple) {
    using namespace network_n;

    auto protocol = Http1_1::instance();
    const std::string STRING_HEADERS =  "HTTP/1.1 200 OK\r\n"
                                        "Content-Type: application/json\r\n"
                                        "Content-Length: 256\r\n"
                                        "Connection: keep-alive\r\n"
                                        "Server: Test/2.4.41";
    auto headersParser = protocol->headersParser();
    Headers expectedHeaders(headersParser);
    expectedHeaders.parse(STRING_HEADERS);
    Body expectedBody(protocol->bodyParser());
    expectedBody.parse(expectedHeaders, this->getTestStringObject());
    const std::string Http1_1_MESSAGE = std::format("{}\r\n\r\n{}", expectedHeaders.toString(), expectedBody.toString()); 

    const auto& [startLineInformation, headers, body] = protocol->parse(Http1_1_MESSAGE);

    EXPECT_EQ(expectedHeaders, headers);
    EXPECT_EQ(headersParser->parseStartLine(expectedHeaders.startLine()), startLineInformation);
    EXPECT_EQ(expectedBody, body);
}

TYPED_TEST(Http1_1Test, MessageToStringWithValidHeadersAndBody_ReturnsValidStringMessage) {
    using namespace network_n;
    auto protocol = Http1_1::instance();
    Headers expectedHeaders(HeadersParserMock::get());
    expectedHeaders.parse("test");
    Body expectedBody(this->getBodyParserMock());
    expectedBody.parse(expectedHeaders, this->getTestStringObject());
    const std::string EXPECTED_STRING_MESSAGE = std::format("{}\r\n\r\n{}", expectedHeaders.toString(), expectedBody.toString()); 

    const std::string result = protocol->messageToString(expectedHeaders, expectedBody);

    EXPECT_EQ(EXPECTED_STRING_MESSAGE, result);
}