#include <gtest/gtest.h>

#include <core/exception.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/network.hpp>
#include <helper/core/build_headers.hpp>
#include <helper/core/headers_parser_mock.hpp>


class HeadersTest : public ::testing::Test {
protected:
    std::shared_ptr<HeadersParserMock> getHeaderParserMockPtr() { return HeadersParserMock::get(); }
};

TEST_F(HeadersTest, ConstructorWithNullptr_ThrowsException) {
    EXPECT_THROW(network_n::Headers(nullptr), InvalidArgument);
}

TEST_F(HeadersTest, SetParserWithNullptr_ThrowsException) {
    EXPECT_THROW(network_n::Headers(this->getHeaderParserMockPtr()).setParser(nullptr), InvalidArgument);
}

TEST_F(HeadersTest, Build_ReturnsValidHeaders) {
    using namespace network_n;

    const std::string EXPECTED_STARTLINE = "HTTP/1.1 200 OK";
    const std::string EXPECTED_HEADER = "Transfer-Encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";
    Headers hs(this->getHeaderParserMockPtr());
    const std::string EXPECTED_RESULT = this->getHeaderParserMockPtr()->build(hs);

    Headers h(this->getHeaderParserMockPtr());
    h.setStartLine(EXPECTED_STARTLINE);
    h.setHeader(EXPECTED_HEADER, EXPECTED_HEADER_VALUE);

    const std::string result = h.build();

    EXPECT_EQ(EXPECTED_RESULT, result);
}

TEST_F(HeadersTest, Parse_ParsesHeadersCorrectly) {
    using namespace network_n;
    const auto& [EXPECTED_STARTLINE, EXPECTED_HEADERS_UMAP, EXPECTED_COOKIES_MAP] = this->getHeaderParserMockPtr()->parse("");
    const std::string EXPECTED_HEADERS = EXPECTED_STARTLINE + std::format("\r\n{}", buildHeaders(EXPECTED_HEADERS_UMAP));

    Headers headers(this->getHeaderParserMockPtr());
    headers.parse(EXPECTED_HEADERS);

    EXPECT_EQ(EXPECTED_HEADERS_UMAP.size(), headers.headersToMap().size());
    for (const auto& [header, expected_value]: EXPECTED_HEADERS_UMAP)
        EXPECT_EQ(expected_value, headers.get(header));
    EXPECT_EQ(EXPECTED_STARTLINE, headers.startLine());
    EXPECT_EQ(EXPECTED_COOKIES_MAP, headers.cookiesToMap());
}

TEST_F(HeadersTest, SetHeader_AddsNewHeader) {
    using namespace network_n;

    const std::string HEADER = "Content-Type";
    const std::string EXPECTED_HEADER_VALUE = "application/json";
    Headers headers(this->getHeaderParserMockPtr());
    const size_t PREVIOUS_SIZE = headers.headersToMap().size();

    headers.setHeader(HEADER, EXPECTED_HEADER_VALUE);

    EXPECT_EQ(EXPECTED_HEADER_VALUE, headers.get(HEADER));
    EXPECT_EQ(PREVIOUS_SIZE + 1, headers.headersToMap().size());
}

TEST_F(HeadersTest, SetHeader_OverwritesExistingHeader) {
    using namespace network_n;

    const std::string HEADER = "Content-Type";
    const std::string OLD_HEADER_VALUE = "application/json";
    const std::string EXPECTED_HEADER_VALUE = "text/html";

    Headers headers(this->getHeaderParserMockPtr());
    headers.setHeader(HEADER, OLD_HEADER_VALUE);
    const size_t PREVIOUS_SIZE = headers.headersToMap().size();
    headers.setHeader(HEADER, EXPECTED_HEADER_VALUE);

    EXPECT_EQ(EXPECTED_HEADER_VALUE, headers.get(HEADER));
    EXPECT_EQ(PREVIOUS_SIZE, headers.headersToMap().size());
}

TEST_F(HeadersTest, GetWithExistingHeader_ReturnsCorrectHeaderValue) {
    using namespace network_n;

    const std::string HEADER = "Content-Type";
    const std::string EXPECTED_HEADER_VALUE = "application/json";

    Headers headers(this->getHeaderParserMockPtr());
    headers.setHeader(HEADER, EXPECTED_HEADER_VALUE);

    EXPECT_EQ(EXPECTED_HEADER_VALUE, headers.get(HEADER));
}

TEST_F(HeadersTest, GetWithNonExistingHeader_ReturnsEmptyString) {
    using namespace network_n;
    const std::string HEADER = "Content-Type";
    const std::string EXPECTED_HEADER_VALUE = "";

    Headers headers(this->getHeaderParserMockPtr());

    EXPECT_EQ(EXPECTED_HEADER_VALUE, headers.get(HEADER));
}

TEST_F(HeadersTest, SetStartLine_AddsNewStartLine) {
    using namespace network_n;
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 200 OK";

    Headers headers(this->getHeaderParserMockPtr());
    headers.setStartLine(EXPECTED_STARTLINE);

    EXPECT_EQ(EXPECTED_STARTLINE, headers.startLine());
}

TEST_F(HeadersTest, SetStartLine_OverwritesExistingStartLine) {
    using namespace network_n;
    const std::string OLD_STARTLINE = "HTTP/1.1 200 OK";
    const std::string EXPECTED_STARTLINE = "HTTP/1.1 404 Not Found";

    Headers headers(this->getHeaderParserMockPtr());
    headers.setStartLine(OLD_STARTLINE);
    headers.setStartLine(EXPECTED_STARTLINE);

    EXPECT_EQ(EXPECTED_STARTLINE, headers.startLine());
}

TEST_F(HeadersTest, headersToMap_ReturnsMapContainingAllHeaders) {
    using namespace network_n;

    const std::unordered_map<std::string, std::string> headersUMap = { {"Accept", "application/xml"},
                                                                       {"Content-Length", "302"},
                                                                       {"Connection", "close"},
                                                                       {"Host", "www.test.com"} };

    Headers headers(this->getHeaderParserMockPtr());
    for (const auto& [header, value] : headersUMap)
        headers.setHeader(header, value);
    

    EXPECT_EQ(headersUMap.size(), headers.headersToMap().size());
    for (const auto& [header, expected_value]: headersUMap)
        EXPECT_EQ(expected_value, headers.get(header));
}