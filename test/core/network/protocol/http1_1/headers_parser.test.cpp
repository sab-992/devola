#include <gtest/gtest.h>

#include <core/exception.h>
#include <core/network/detail/headers.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/network.h>
#include <utils/core/headers_parser_mock.h>

class HeadersParserTest : public ::testing::Test {};

TEST_F(HeadersParserTest, Instance_ReturnsValidPointer) {
    using network_n::protocol_n::http1_1_n::HeadersParser;
    EXPECT_NE(nullptr, HeadersParser::instance());
}

TEST_F(HeadersParserTest, Instance_AlwaysReturnsTheSamePointer) {
    using network_n::protocol_n::http1_1_n::HeadersParser;
    auto EXPECTED = HeadersParser::instance();
    EXPECT_EQ(EXPECTED, HeadersParser::instance());
}

TEST_F(HeadersParserTest, BuildWithStartlineAndWithoutHeaders_ReturnsStartline) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 200 OK";
    auto parser = HeadersParser::instance();
    Headers h(parser);
    h.setStartLine(EXPECTED_STARTLINE);

    const std::string result = parser->build(h);

    EXPECT_EQ(EXPECTED_STARTLINE, result);
}

TEST_F(HeadersParserTest, BuildWithStartlineAndHeaders_ReturnsValidHTTPHeaders) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 200 OK";
    const std::string EXPECTED_HEADER = "Transfer-Encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";
    const std::string EXPECTED_RESULT = std::format("{}\r\n{}: {}", EXPECTED_STARTLINE, EXPECTED_HEADER, EXPECTED_HEADER_VALUE);
    auto parser = HeadersParser::instance();
    Headers h(parser);
    h.setStartLine(EXPECTED_STARTLINE);
    h.setHeader(EXPECTED_HEADER, EXPECTED_HEADER_VALUE);

    const std::string result = parser->build(h);

    EXPECT_EQ(EXPECTED_RESULT, result);
}

TEST_F(HeadersParserTest, BuildWithHeadersAndWithoutStartlineAnd_ReturnsHeaders) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 200 OK";
    const std::string EXPECTED_HEADER = "Transfer-Encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";
    const std::string EXPECTED_RESULT = std::format("{}\r\n{}: {}", EXPECTED_STARTLINE, EXPECTED_HEADER, EXPECTED_HEADER_VALUE);
    auto parser = HeadersParser::instance();
    Headers h(parser);
    h.setStartLine(EXPECTED_STARTLINE);
    h.setHeader(EXPECTED_HEADER, EXPECTED_HEADER_VALUE);

    const std::string result = parser->build(h);

    EXPECT_EQ(EXPECTED_RESULT, result);
}

TEST_F(HeadersParserTest, BuildWithoutStartlineAndHeadersUMap_ReturnsEmptyString) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_RESULT = "";
    auto parser = HeadersParser::instance();
    Headers h(parser);

    const std::string result = parser->build(h);

    EXPECT_EQ(EXPECTED_RESULT, result);
}

TEST_F(HeadersParserTest, IsContentChunkedWithChunkedInHeaders_ReturnsTrue) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_HEADER = "Transfer-Encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";
    Headers h(HeadersParserMock::get(true));
    h.setHeader(EXPECTED_HEADER, EXPECTED_HEADER_VALUE);

    const bool result = HeadersParser::isContentChunked(h);

    EXPECT_TRUE(result);
}

TEST_F(HeadersParserTest, IsContentChunkedWithoutChunkedInHeaders_ReturnsFalse) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_HEADER = "Transfer-Encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";
    Headers h(HeadersParserMock::get());

    const bool result = HeadersParser::isContentChunked(h);

    EXPECT_FALSE(result);
}


TEST_F(HeadersParserTest, ParseWithValidStartlineAndHeaders_ReturnsStartlineAndHeadersMap) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 200 OK";
    const std::string EXPECTED_HEADER = "Transfer-Encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";
    const std::pair<std::string, headersUMap_t> EXPECTED_HTTP_HEADERS = { EXPECTED_STARTLINE, { {EXPECTED_HEADER, EXPECTED_HEADER_VALUE } } };
    auto parser = HeadersParser::instance();
    Headers h(parser);
    h.setStartLine(EXPECTED_STARTLINE);

    const std::pair<std::string, headersUMap_t> result = parser->parse(std::format("{}\r\n{}: {}", EXPECTED_STARTLINE, EXPECTED_HEADER, EXPECTED_HEADER_VALUE));

    EXPECT_EQ(EXPECTED_HTTP_HEADERS, result);
}

TEST_F(HeadersParserTest, ParseWithoutValidStartline_ThrowsException) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 OK";
    const std::pair<std::string, headersUMap_t> EXPECTED_HTTP_HEADERS = { EXPECTED_STARTLINE, { } };
    auto parser = HeadersParser::instance();
    Headers h(parser);
    h.setStartLine(EXPECTED_STARTLINE);

    EXPECT_THROW(parser->parse(EXPECTED_STARTLINE), InvalidArgument);
}

TEST_F(HeadersParserTest, ParseWithoutValidHeader_ThrowsException) {
    using namespace network_n;
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 200 OK";
    const std::string EXPECTED_HEADER = "Transfer-Encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";
    const std::pair<std::string, headersUMap_t> EXPECTED_HTTP_HEADERS = { EXPECTED_STARTLINE, { {EXPECTED_HEADER, EXPECTED_HEADER_VALUE } } };
    auto parser = HeadersParser::instance();
    Headers h(parser);
    h.setStartLine(EXPECTED_STARTLINE);

    EXPECT_THROW(parser->parse(std::format("{}\r\n{}", EXPECTED_STARTLINE, EXPECTED_HEADER)), InvalidArgument);
}

TEST_F(HeadersParserTest, ParseStartlineWithRequestStartline_ReturnsResponseStartLineInformationMap) {
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string REQUEST_STARTLINE = "GET /test HTTP/1.1";
    const startLineInformation_t EXPECTED_REQUEST_INFORMATION = { "GET", "/test", "HTTP/1.1" };

    const startLineInformation_t result = HeadersParser::instance()->parseStartLine(REQUEST_STARTLINE);

    EXPECT_EQ(EXPECTED_REQUEST_INFORMATION, result);
}

TEST_F(HeadersParserTest, ParseStartlineWithResponseStartline_ReturnsResponseStartLineInformationMap) {
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string RESPONSE_STARTLINE = "HTTP/1.1 200 OK";
    const startLineInformation_t EXPECTED_RESPONSE_INFORMATION = { "HTTP/1.1", "200", "OK" };

    const startLineInformation_t result = HeadersParser::instance()->parseStartLine(RESPONSE_STARTLINE);

    EXPECT_EQ(EXPECTED_RESPONSE_INFORMATION, result);
}

TEST_F(HeadersParserTest, ParseStartlineWithoutEnoughInformation_ThrowsException) {
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string INVALID_STARTLINE = "HTTP/1.1 200";
    EXPECT_THROW(HeadersParser::instance()->parseStartLine(INVALID_STARTLINE), InvalidArgument);
}

TEST_F(HeadersParserTest, ParseStartlineWithoutRequestOrResponseStartline_ThrowsException) {
    using network_n::protocol_n::http1_1_n::HeadersParser;
    const std::string INVALID_STARTLINE = "GET HTTP/1.1 20easd0";
    EXPECT_THROW(HeadersParser::instance()->parseStartLine(INVALID_STARTLINE), InvalidArgument);
}