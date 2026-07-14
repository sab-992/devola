#include <gtest/gtest.h>

#include <core/exception.hpp>
#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/http/detail/version/http1_1/body_parser.hpp>
#include <core/network/network.hpp>
#include <helper/core/headers_parser_mock.hpp>
#include <helper/core/inner_types.hpp>

using namespace http_n::version_n::http1_1_n;
using network_n::Headers;

Headers getMockedHeaders(bool isChunked=false, bool isDownload=false) {
    Headers headers(HeadersParserMock::get(isChunked, isDownload));
    headers.parse("Hello world");
    return std::move(headers);
}

template<typename T>
class BodyParserTypedTest : public ::testing::Test {
protected:
    auto getTestObject(bool alt=false) {
        return InnerTypes<T>::getTestObject(InnerTypes<T>::getTestString(alt));
    }

    std::string getTestStringObject(bool alt=false) {
        return InnerTypes<T>::getTestStringFromObject(this->getTestObject(alt));
    }
};

TYPED_TEST_SUITE(BodyParserTypedTest, innerTypes_t);

TYPED_TEST(BodyParserTypedTest, Instance_ReturnsValidPointer) {
    EXPECT_NE(nullptr, BodyParser::instance());
}

TYPED_TEST(BodyParserTypedTest, Instance_AlwaysReturnsTheSamePointer) {
    auto EXPECTED = BodyParser::instance();
    EXPECT_EQ(EXPECTED, BodyParser::instance());
}

TYPED_TEST(BodyParserTypedTest, BuildWithoutChunkedHeader_ReturnsBody) {
    using namespace network_n;

    const size_t EXPECTED_CHUNKS_COUNT = 1;
    std::shared_ptr<BodyParser> parser = BodyParser::instance();
    Body body(parser);
    body.set<TypeParam>(this->getTestObject());

    const std::vector<std::string> result = parser->build(getMockedHeaders(), body);

    EXPECT_EQ(EXPECTED_CHUNKS_COUNT, result.size());
    EXPECT_EQ(this->getTestStringObject(), result[0]);
}

TYPED_TEST(BodyParserTypedTest, ParseWithoutChunkedHeader_ReturnsStringBody) {
    using namespace network_n;

    const std::string EXPECTED_PARSED_RESULT = this->getTestStringObject();
    const std::string result = BodyParser::instance()->parse(getMockedHeaders(), EXPECTED_PARSED_RESULT);

    EXPECT_EQ(EXPECTED_PARSED_RESULT, result);
}


class BodyParserTest : public ::testing::Test {
protected:
    std::string getBodyToChunk(size_t sizeOfChunk) { return std::string(sizeOfChunk + 1, '*'); }
};

TEST_F(BodyParserTest, BuildWithChunkedHeaderAndBodySmallerThanChunkSize_ReturnsChunkedBodyAsSingleChunk) {
    using namespace network_n;
    using namespace http_n::version_n::http1_1_n;

    const std::string EXPECTED_STRING_BODY = "Hello world";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_STRING_BODY.size()), EXPECTED_STRING_BODY), "0\r\n\r\n" };

    std::shared_ptr<BodyParser> parser = BodyParser::instance();

    Body body(parser);
    body.set<std::string>(EXPECTED_STRING_BODY);

    const std::vector<std::string> result = parser->build(getMockedHeaders(true), body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, BuildWithChunkedHeaderAndDownloadAndBodySmallerThanChunkSize_ReturnsChunkedBodyAsSingleChunk) {
    using namespace network_n;
    using namespace http_n::version_n::http1_1_n;

    const std::string EXPECTED_STRING_BODY = "Hello world";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_STRING_BODY.size()), EXPECTED_STRING_BODY), "0\r\n\r\n" };

    std::shared_ptr<BodyParser> parser = BodyParser::instance();

    Body body(parser);
    body.set<std::string>(EXPECTED_STRING_BODY);

    const std::vector<std::string> result = parser->build(getMockedHeaders(true, true), body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, BuildWithChunkedHeader_ReturnsMultipleBodyChunks) {
    using namespace network_n;
    using namespace http_n::version_n::http1_1_n;

    const std::string EXPECTED_FIRST_CHUNK = std::string(REQUEST_BUFFER_MAX_SIZE, '*');
    const std::string EXPECTED_SECOND_CHUNK = "*";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_FIRST_CHUNK.size()), EXPECTED_FIRST_CHUNK),
                                                     std::format("{}\r\n{}", toHex(EXPECTED_SECOND_CHUNK.size()), EXPECTED_SECOND_CHUNK),
                                                     "0\r\n\r\n" };

    std::shared_ptr<BodyParser> parser = BodyParser::instance();

    Body body(parser);
    body.set<std::string>(this->getBodyToChunk(REQUEST_BUFFER_MAX_SIZE));

    const std::vector<std::string> result = parser->build(getMockedHeaders(true), body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, BuildWithChunkedHeaderAndDownload_ReturnsMultipleBiggerBodyChunks) {
    using namespace network_n;
    using namespace http_n::version_n::http1_1_n;

    const std::string EXPECTED_FIRST_CHUNK = std::string(DOWNLOAD_BUFFER_MAX_SIZE, '*');
    const std::string EXPECTED_SECOND_CHUNK = "*";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_FIRST_CHUNK.size()), EXPECTED_FIRST_CHUNK),
                                                     std::format("{}\r\n{}", toHex(EXPECTED_SECOND_CHUNK.size()), EXPECTED_SECOND_CHUNK),
                                                     "0\r\n\r\n" };

    std::shared_ptr<BodyParser> parser = BodyParser::instance();

    Body body(parser);
    body.set<std::string>(this->getBodyToChunk(DOWNLOAD_BUFFER_MAX_SIZE));

    const std::vector<std::string> result = parser->build(getMockedHeaders(true, true), body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, ParseWithChunkedHeader_ReturnsMergedBody) {
    using namespace http_n::version_n::http1_1_n;

    const std::string EXPECTED_PARSED_RESULT = "Hello world";

    const std::string result = BodyParser::instance()->parse(getMockedHeaders(true), "5\r\n"
                                                                                     "Hello\r\n"
                                                                                     "6\r\n"
                                                                                     " world\r\n"
                                                                                     "0\r\n\r\n");

    EXPECT_EQ(EXPECTED_PARSED_RESULT, result);
}

TEST_F(BodyParserTest, ParseWithChunkedHeaderAndDownload_ReturnsMergedBody) {
    using namespace http_n::version_n::http1_1_n;

    const std::string EXPECTED_PARSED_RESULT = "Hello world2";

    const std::string result = BodyParser::instance()->parse(getMockedHeaders(true, true), "5\r\n"
                                                                                           "Hello\r\n"
                                                                                           "7\r\n"
                                                                                           " world2\r\n"
                                                                                           "0\r\n\r\n");

    EXPECT_EQ(EXPECTED_PARSED_RESULT, result);
}

TEST_F(BodyParserTest, ParseWithIncorrectStringBody_ThrowsException) {
    using namespace http_n::version_n::http1_1_n;

    const std::string POORLY_CHUNKED_BODY = "5"
                                            "Hello\r\n"
                                            "6\r\n"
                                            " world\r\n"
                                            "0\r\n\r\n";

    EXPECT_THROW(BodyParser::instance()->parse(getMockedHeaders(true, true), POORLY_CHUNKED_BODY), InvalidArgument);
}