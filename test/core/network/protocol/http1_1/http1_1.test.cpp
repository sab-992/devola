#include <gtest/gtest.h>

#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/detail/protocol/http1_1/http1_1.hpp>
#include <utils/core/body_parser_mock.hpp>
#include <utils/core/headers_parser_mock.hpp>
#include <utils/core/inner_types.hpp>

template<typename T>
class Http1_1Test : public ::testing::Test {
public:
    template <typename ContentType>
    struct TemplatedBody {
        using type = network_n::protocol_n::Http1_1<ContentType>;
    };

    using Inner = InnerTemplatedTypes<T, Http1_1Test<T>::template TemplatedBody>;

protected:
    auto getTestObject(bool alt=false) {
        return Inner::getTestObject(getTestStringBody(alt));
    }

    std::string getTestStringBody(bool alt=false) {
        return Inner::getTestStringObject(alt);
    }

    auto getBodyParserMock(bool alt=false) {
        return Inner::getBodyParserMock(alt);
    }
};

TYPED_TEST_SUITE(Http1_1Test, networkTemplatedInnerTypes_t<network_n::protocol_n::Http1_1>);

TYPED_TEST(Http1_1Test, Packetize_ReturnsVectorContainingPacketsToSend) {
    using namespace network_n;

    auto headerParser = HeadersParserMock::get();
    auto bodyParser = this->getBodyParserMock();
    Headers headers(headerParser);
    Body<decltype(this->getTestObject())> body(bodyParser);
    const std::vector<std::string> EXPECTED_PACKETS = { headerParser->build(headers), bodyParser->build(headers, body)[0] };
    auto protocol = TypeParam::instance();

    const std::vector<std::string> result = protocol->packetize(headers, body);

    EXPECT_EQ(EXPECTED_PACKETS, result);
}

TYPED_TEST(Http1_1Test, Parse_ReturnsStartLineInformationAndHeadersAndBodyObjectTuple) {
    using namespace network_n;

    auto protocol = TypeParam::instance();
    const std::string STRING_HEADERS =  "HTTP/1.1 200 OK\r\n"
                                        "Content-Type: application/json\r\n"
                                        "Content-Length: 256\r\n"
                                        "Connection: keep-alive\r\n"
                                        "Server: Test/2.4.41";
    auto headersParser = protocol->headersParser();
    Headers expected_headers(headersParser);
    expected_headers.parse(STRING_HEADERS);
    Body<decltype(this->getTestObject())> expected_body(protocol->bodyParser());
    expected_body.parse(expected_headers, this->getTestStringBody());
    const std::string Http1_1_MESSAGE = std::format("{}\r\n\r\n{}", expected_headers.toString(), expected_body.toString()); 

    const auto& [startLineInformation, headers, body] = protocol->parse(Http1_1_MESSAGE);

    EXPECT_EQ(expected_headers, headers);
    EXPECT_EQ(headersParser->parseStartLine(expected_headers.startLine()), startLineInformation);
    EXPECT_EQ(expected_body, body);
}

TYPED_TEST(Http1_1Test, MessageToStringWithValidHeadersAndBody_ReturnsValidStringMessage) {
    using namespace network_n;
    auto protocol = TypeParam::instance();
    Headers expected_headers(HeadersParserMock::get());
    expected_headers.parse("test");
    Body<decltype(this->getTestObject())> expected_body(this->getBodyParserMock());
    expected_body.parse(expected_headers, this->getTestStringBody());
    const std::string EXPECTED_STRING_MESSAGE = std::format("{}\r\n\r\n{}", expected_headers.toString(), expected_body.toString()); 

    const std::string result = protocol->messageToString(expected_headers, expected_body);

    EXPECT_EQ(EXPECTED_STRING_MESSAGE, result);
}