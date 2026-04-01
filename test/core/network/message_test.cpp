#include <gtest/gtest.h>

#include <core/http/request.h>
#include <core/http/response.h>
#include <core/xml/xml.h>
#include <nlohmann/json.hpp>


template<typename T>
class MessageTest : public ::testing::Test {
protected:
    auto GetTestBody(bool alternate=false) {
        using network_n::Message;
        if constexpr (std::is_base_of_v<Message<T, nlohmann::json>, T>)
            return createJSON(alternate ? R"({ "alternateTest": "works!", "json": true })" : R"({ "test": "works!", "json": true })");
        if constexpr (std::is_base_of_v<Message<T, std::string>, T>)
            return std::string(alternate ? "Hello alternate test!" : "Hello test!");
        if constexpr (std::is_base_of_v<Message<T, xml_n::Document>, T>)
            return createXML(alternate ? "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello alternate xml test!</item>\n</root>\n" : 
                                         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello xml test!</item>\n</root>\n");
        throw std::invalid_argument("Unhandled type in MessageTest::GetBody");
    }

private:
    nlohmann::json createJSON(const std::string& content) {
        return nlohmann::json::parse(content);
    }

    xml_n::Document createXML(const std::string& content) {
        return xml_n::Document(content);
    }
};

TYPED_TEST_SUITE_P(MessageTest);

TYPED_TEST_P(MessageTest, SetBody_AddsNewBody) {
    TypeParam message;
    const auto EXPECTED_BODY = this->GetTestBody();

    message.setBody(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, message.body());
}

TYPED_TEST_P(MessageTest, SetBody_OverwritesExistingBody) {
    TypeParam message;
    const auto ALTERNATE_BODY = this->GetTestBody(true);
    const auto EXPECTED_BODY = this->GetTestBody();

    message.setBody(ALTERNATE_BODY);
    message.setBody(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, message.body());
}

TYPED_TEST_P(MessageTest, Body_IsConvertedCorrectly) {
    TypeParam message;
    const auto EXPECTED_BODY = this->GetTestBody();

    message.setBody(EXPECTED_BODY);

    EXPECT_TRUE(typeid(EXPECTED_BODY) == typeid(message.body()));
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

template <template <typename> class Message>
using Messages = ::testing::Types<Message<std::string>, Message<nlohmann::json>, Message<xml_n::Document>>;

INSTANTIATE_TYPED_TEST_SUITE_P(HTTPRequest, MessageTest, Messages<http_n::Request>);
INSTANTIATE_TYPED_TEST_SUITE_P(HTTPResponse, MessageTest, Messages<http_n::Response>);