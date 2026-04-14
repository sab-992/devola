#include <gtest/gtest.h>

#include <core/exception.h>
#include <core/http/request.h>
#include <core/http/response.h>
#include <core/xml/xml.h>
#include <nlohmann/json.hpp>
#include <utils/core/inner_types.h>


template<typename MessageType>
class MessageTest : public ::testing::Test {
public:
    template<typename ContentType>
    struct TemplatedMessage {
        using type = network_n::Message<MessageType, ContentType>;
    };

protected:
    auto getTestBody(bool alt=false) {
        return InnerTypes<MessageType, MessageTest<MessageType>::template TemplatedMessage>::getTestBody(alt);
    }
};

TYPED_TEST_SUITE_P(MessageTest);

TYPED_TEST_P(MessageTest, SetBody_AddsNewBody) {
    TypeParam message;
    const auto EXPECTED_BODY = this->getTestBody();

    message.setBody(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, message.body());
}

TYPED_TEST_P(MessageTest, SetBody_OverwritesExistingBody) {
    TypeParam message;
    const auto ALTERNATE_BODY = this->getTestBody(true /* alt */);
    const auto EXPECTED_BODY = this->getTestBody();

    message.setBody(ALTERNATE_BODY);
    message.setBody(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, message.body());
}

TYPED_TEST_P(MessageTest, Body_IsConvertedCorrectly) {
    TypeParam message;
    const auto EXPECTED_BODY = this->getTestBody();

    message.setBody(EXPECTED_BODY);

    EXPECT_EQ(typeid(EXPECTED_BODY), typeid(message.body()));
}

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

REGISTER_TYPED_TEST_SUITE_P(MessageTest, SetBody_AddsNewBody,
                                         SetBody_OverwritesExistingBody,
                                         Body_IsConvertedCorrectly,
                                         SetHeader_AddsNewHeader,
                                         SetHeader_IncrementsSize,
                                         SetHeader_OverwritesExistingHeader);

INSTANTIATE_TYPED_TEST_SUITE_P(HTTPRequestTestSuite, MessageTest, networkInnerTypes_t<http_n::Request>);
INSTANTIATE_TYPED_TEST_SUITE_P(HTTPResponseTestSuite, MessageTest, networkInnerTypes_t<http_n::Response>);