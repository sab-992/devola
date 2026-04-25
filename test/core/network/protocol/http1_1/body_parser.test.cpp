#include <gtest/gtest.h>

#include <core/exception.h>
#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/detail/protocol/http1_1/body_parser.h>
#include <core/network/network.h>
#include <utils/core/headers_parser_mock.h>
#include <utils/core/inner_types.h>


template<typename T>
class BodyParserTypedTest : public ::testing::Test {
public:
    template <typename ContentType>
    struct TemplatedBody {
        using type = network_n::protocol_n::http1_1_n::BodyParser<ContentType>;
    };

    using Inner = InnerTypes<T, BodyParserTypedTest<T>::template TemplatedBody>;

protected:
    auto getTestBody(bool alt=false) {
        return Inner::getTestObject(getTestStringBody(alt));
    }

    std::string getTestStringBody(bool alt=false) {
        return Inner::getTestStringObject(alt);
    }
};

TYPED_TEST_SUITE(BodyParserTypedTest, networkInnerTypes_t<network_n::protocol_n::http1_1_n::BodyParser>);

TYPED_TEST(BodyParserTypedTest, Instance_ReturnsValidPointer) {
    EXPECT_NE(nullptr, TypeParam::instance());
}

TYPED_TEST(BodyParserTypedTest, Instance_AlwaysReturnsTheSamePointer) {
    auto EXPECTED = TypeParam::instance();
    EXPECT_EQ(EXPECTED, TypeParam::instance());
}

TYPED_TEST(BodyParserTypedTest, BuildWithoutChunkedHeader_ReturnsBody) {
    using namespace network_n;

    const size_t EXPECTED_CHUNKS_COUNT = 1;
    Headers headers(HeadersParserMock::get(false));
    headers.parse("Hello world");
    auto bodyContent = this->getTestBody();
    auto parser = TypeParam::instance();
    Body<decltype(bodyContent)> body(parser);
    body.set(bodyContent);

    const std::vector<std::string> result = parser->build(headers, body);

    EXPECT_EQ(EXPECTED_CHUNKS_COUNT, result.size());
    EXPECT_EQ(this->getTestStringBody(), result[0]);
}

TYPED_TEST(BodyParserTypedTest, ParseWithoutChunkedHeader_ReturnsBody) {
    using namespace network_n;

    const std::string EXPECTED_PARSED_RESULT = this->getTestStringBody();
    Headers headers(HeadersParserMock::get(false));
    headers.parse("Hello world");
    const std::string result = TypeParam::instance()->parse(headers, EXPECTED_PARSED_RESULT);

    EXPECT_EQ(EXPECTED_PARSED_RESULT, result);
}


class BodyParserTest : public ::testing::Test {
protected:
    std::string getBodyToChunk(size_t sizeOfChunk) { return std::string(sizeOfChunk + 1, '*'); }
};

TEST_F(BodyParserTest, BuildWithChunkedHeaderAndBodySmallerThanChunkSize_ReturnsSingleBodyChunk) {
    using namespace network_n;
    using protocol_n::http1_1_n::BodyParser;

    const std::string EXPECTED_STRING_BODY = "Hello world";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_STRING_BODY.size()), EXPECTED_STRING_BODY), "0\r\n\r\n" };

    Headers headers(HeadersParserMock::get(true));
    headers.parse("Hello world");
    std::shared_ptr<BodyParser<std::string>> parser = BodyParser<std::string>::instance();

    Body<std::string> body(parser);
    body.set(EXPECTED_STRING_BODY);

    const std::vector<std::string> result = parser->build(headers, body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, BuildWithChunkedHeaderAndDownloadAndBodySmallerThanChunkSize_ReturnsSingleBodyChunk) {
    using namespace network_n;
    using protocol_n::http1_1_n::BodyParser;

    const std::string EXPECTED_STRING_BODY = "Hello world";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_STRING_BODY.size()), EXPECTED_STRING_BODY), "0\r\n\r\n" };

    Headers headers(HeadersParserMock::get(true, true));
    headers.parse("Hello world");
    std::shared_ptr<BodyParser<std::string>> parser = BodyParser<std::string>::instance();

    Body<std::string> body(parser);
    body.set(EXPECTED_STRING_BODY);

    const std::vector<std::string> result = parser->build(headers, body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, BuildWithChunkedHeader_ReturnsMultipleBodyChunks) {
    using namespace network_n;
    using protocol_n::http1_1_n::BodyParser;

    const std::string EXPECTED_FIRST_CHUNK = std::string(REQUEST_BUFFER_MAX_SIZE, '*');
    const std::string EXPECTED_SECOND_CHUNK = "*";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_FIRST_CHUNK.size()), EXPECTED_FIRST_CHUNK), 
                                                     std::format("{}\r\n{}", toHex(EXPECTED_SECOND_CHUNK.size()), EXPECTED_SECOND_CHUNK),
                                                     "0\r\n\r\n" };

    Headers headers(HeadersParserMock::get(true));
    headers.parse("Hello world");
    std::shared_ptr<BodyParser<std::string>> parser = BodyParser<std::string>::instance();

    Body<std::string> body(parser);
    body.set(this->getBodyToChunk(REQUEST_BUFFER_MAX_SIZE));

    const std::vector<std::string> result = parser->build(headers, body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, BuildWithChunkedHeaderAndDownload_ReturnsMultipleBiggerBodyChunks) {
    using namespace network_n;
    using protocol_n::http1_1_n::BodyParser;

    const std::string EXPECTED_FIRST_CHUNK = std::string(DOWNLOAD_BUFFER_MAX_SIZE, '*');
    const std::string EXPECTED_SECOND_CHUNK = "*";
    const std::vector<std::string> EXPECTED_CHUNKS { std::format("{}\r\n{}", toHex(EXPECTED_FIRST_CHUNK.size()), EXPECTED_FIRST_CHUNK), 
                                                     std::format("{}\r\n{}", toHex(EXPECTED_SECOND_CHUNK.size()), EXPECTED_SECOND_CHUNK),
                                                     "0\r\n\r\n" };

    Headers headers(HeadersParserMock::get(true, true));
    headers.parse("Hello world");
    std::shared_ptr<BodyParser<std::string>> parser = BodyParser<std::string>::instance();

    Body<std::string> body(parser);
    body.set(this->getBodyToChunk(DOWNLOAD_BUFFER_MAX_SIZE));

    const std::vector<std::string> result = parser->build(headers, body);

    EXPECT_EQ(EXPECTED_CHUNKS, result);
}

TEST_F(BodyParserTest, ParseWithChunkedHeader_ReturnsMergedBody) {
    using namespace network_n;
    using protocol_n::http1_1_n::BodyParser;

    const std::string EXPECTED_PARSED_RESULT = "Hello world";

    Headers headers(HeadersParserMock::get(true));
    headers.parse("Hello world");
    const std::string result = BodyParser<std::string>::instance()->parse(headers,  "5\r\n"
                                                                                    "Hello\r\n"
                                                                                    "6\r\n"
                                                                                    " world\r\n"
                                                                                    "0\r\n\r\n");

    EXPECT_EQ(EXPECTED_PARSED_RESULT, result);
}

TEST_F(BodyParserTest, ParseWithChunkedHeaderAndDownload_ReturnsMergedBody) {
    using namespace network_n;
    using protocol_n::http1_1_n::BodyParser;

    const std::string EXPECTED_PARSED_RESULT = "Hello world2";

    Headers headers(HeadersParserMock::get(true, true));
    headers.parse("Hello world");
    const std::string result = BodyParser<std::string>::instance()->parse(headers,  "5\r\n"
                                                                                    "Hello\r\n"
                                                                                    "7\r\n"
                                                                                    " world2\r\n"
                                                                                    "0\r\n\r\n");

    EXPECT_EQ(EXPECTED_PARSED_RESULT, result);
}

TEST_F(BodyParserTest, ParseWithIncorrectStringBody_ThrowsException) {
    using namespace network_n;
    using protocol_n::http1_1_n::BodyParser;

    const std::string POORLY_CHUNKED_BODY = "5"
                                            "Hello\r\n"
                                            "6\r\n"
                                            " world\r\n"
                                            "0\r\n\r\n";

    Headers headers(HeadersParserMock::get(true, true));
    headers.parse("Hello world");
    EXPECT_THROW(BodyParser<std::string>::instance()->parse(headers,  POORLY_CHUNKED_BODY), InvalidArgument);
}