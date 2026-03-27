#include <gtest/gtest.h>

#include <core/http/request.h>
#include <core/http/response.h>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>


template<typename T>
class MessageTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(MessageTest);

TYPED_TEST_P(MessageTest, SetHeader_AddsNewHeader) {
    TypeParam message;

    const std::string EXPECTED_HEADER_NAME = "Transfer-encoding";
    const std::string EXPECTED_HEADER_VALUE = "chunked";

    message.setHeader(EXPECTED_HEADER_NAME, EXPECTED_HEADER_VALUE);

    EXPECT_EQ(EXPECTED_HEADER_VALUE, message.header(EXPECTED_HEADER_NAME));
}

TYPED_TEST_P(MessageTest, SetHeader_IncrementsSize) {
    TypeParam message;

    const size_t EXPECTED_MAP_SIZE_INCREASE = 1;
    const size_t EXPECTED_HEADERS_MAP_SIZE = 3;

    message.setHeader("Accept", "text/html");
    message.setHeader("Content-Length", "8");
    const size_t PREVIOUS_MAP_SIZE = message.headersMap().size();
    message.setHeader("Transfer-encoding", "chunked");

    EXPECT_EQ(EXPECTED_MAP_SIZE_INCREASE, message.headersMap().size() - PREVIOUS_MAP_SIZE);
    EXPECT_EQ(EXPECTED_HEADERS_MAP_SIZE, message.headersMap().size());
}

TYPED_TEST_P(MessageTest, SetHeader_OverwritesExistingHeader) {
    TypeParam message;

    const std::string EXPECTED_HEADER_NAME = "Accept";
    const std::string EXPECTED_HEADER_VALUE = "text/html";
    const size_t EXPECTED_HEADERS_MAP_SIZE = 1;

    message.setHeader(EXPECTED_HEADER_NAME, "application/json");
    message.setHeader(EXPECTED_HEADER_NAME, EXPECTED_HEADER_VALUE);

    EXPECT_EQ(EXPECTED_HEADERS_MAP_SIZE, message.headersMap().size());
    EXPECT_EQ(EXPECTED_HEADER_VALUE, message.header(EXPECTED_HEADER_NAME));
}

REGISTER_TYPED_TEST_SUITE_P(MessageTest, SetHeader_AddsNewHeader,
                                         SetHeader_IncrementsSize,
                                         SetHeader_OverwritesExistingHeader);


template <template <typename> class Message>
using Messages = ::testing::Types<Message<std::string>, Message<nlohmann::json>, Message<pugi::xml_document>>;

INSTANTIATE_TYPED_TEST_SUITE_P(HTTPRequest, MessageTest, Messages<http_n::Request>);
INSTANTIATE_TYPED_TEST_SUITE_P(HTTPResponse, MessageTest, Messages<http_n::Response>);