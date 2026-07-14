#include <gtest/gtest.h>

#include <core/exception.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <core/xml/document.hpp>
#include <nlohmann/json.hpp>
#include <helper/core/inner_types.hpp>
#include <helper/core/template_iteration.hpp>

using http_n::Request;
using http_n::Response;

using messageTypes_t = ::testing::Types<Request, Response>;

template<typename MessageType>
class MessageTest : public ::testing::Test {
protected:
    template <typename T>
    auto getTestObject(bool alt=false) {
        return InnerTypes<T>::getTestObject(InnerTypes<T>::getTestString(alt));
    }

    template <typename T>
    std::string getTestStringObject(bool alt=false) {
        return InnerTypes<T>::getTestStringFromObject(this->template getTestObject<T>(alt));
    }
};

TYPED_TEST_SUITE(MessageTest, messageTypes_t);

TYPED_TEST(MessageTest, SetBody_AddsNewBody) {
    ForEachType<innerTypes_t>([&]<typename T>() {
        SCOPED_TRACE(std::format("Body type: {}", typeid(T).name()));
        TypeParam message;
        const auto EXPECTED_BODY = this->template getTestObject<T>();

        message.template setBody<T>(EXPECTED_BODY);

        EXPECT_EQ(EXPECTED_BODY, message.template body<T>());
    });
}

TYPED_TEST(MessageTest, SetBody_OverwritesExistingBody) {
    ForEachType<innerTypes_t>([&]<typename T>() {
        SCOPED_TRACE(std::format("Body type: {}", typeid(T).name()));
        TypeParam message;
        const auto ALTERNATE_BODY = this->template getTestObject<T>(true /* alt */);
        const auto EXPECTED_BODY = this->template getTestObject<T>();

        message.template setBody<T>(ALTERNATE_BODY);
        message.template setBody<T>(EXPECTED_BODY);

        EXPECT_EQ(EXPECTED_BODY, message.template body<T>());
    });
}

TYPED_TEST(MessageTest, Body_IsConvertedCorrectly) {
    ForEachType<innerTypes_t>([&]<typename T>() {
        SCOPED_TRACE(std::format("Body type: {}", typeid(T).name()));
        TypeParam message;
        const auto EXPECTED_BODY = this->template getTestObject<T>();

        message.template setBody<T>(EXPECTED_BODY);

        EXPECT_EQ(typeid(EXPECTED_BODY), typeid(message.template body<T>()));
    });
}

TYPED_TEST(MessageTest, SetHeader_AddsNewHeader) {
    ForEachType<innerTypes_t>([&]<typename T>() {
        SCOPED_TRACE(std::format("Body type: {}", typeid(T).name()));
        TypeParam message;
        const std::string EXPECTED_HEADER_NAME = "Transfer-encoding";
        const std::string EXPECTED_HEADER_VALUE = "chunked";

        message.setHeader(EXPECTED_HEADER_NAME, EXPECTED_HEADER_VALUE);

        EXPECT_EQ(EXPECTED_HEADER_VALUE, message.header(EXPECTED_HEADER_NAME));
    });
}

TYPED_TEST(MessageTest, SetHeader_IncrementsSize) {
    ForEachType<innerTypes_t>([&]<typename T>() {
        SCOPED_TRACE(std::format("Body type: {}", typeid(T).name()));
        TypeParam message;
        const size_t EXPECTED_MAP_SIZE_INCREASE = 1;
        const size_t EXPECTED_HEADERS_MAP_SIZE = 3;

        message.setHeader("Accept", "text/html");
        message.setHeader("Content-Length", "8");
        const size_t PREVIOUS_MAP_SIZE = message.headersMap().size();
        message.setHeader("Transfer-encoding", "chunked");

        EXPECT_EQ(EXPECTED_MAP_SIZE_INCREASE, message.headersMap().size() - PREVIOUS_MAP_SIZE);
        EXPECT_EQ(EXPECTED_HEADERS_MAP_SIZE, message.headersMap().size());
    });
}

TYPED_TEST(MessageTest, SetHeader_OverwritesExistingHeader) {
    ForEachType<innerTypes_t>([&]<typename T>() {
        SCOPED_TRACE(std::format("Body type: {}", typeid(T).name()));
        TypeParam message;
        const std::string EXPECTED_HEADER_NAME = "Accept";
        const std::string EXPECTED_HEADER_VALUE = "text/html";
        const size_t EXPECTED_HEADERS_MAP_SIZE = 1;

        message.setHeader(EXPECTED_HEADER_NAME, "application/json");
        message.setHeader(EXPECTED_HEADER_NAME, EXPECTED_HEADER_VALUE);

        EXPECT_EQ(EXPECTED_HEADERS_MAP_SIZE, message.headersMap().size());
        EXPECT_EQ(EXPECTED_HEADER_VALUE, message.header(EXPECTED_HEADER_NAME));
    });
}

TYPED_TEST(MessageTest, SetProtocolWithoutProtocol_ThrowsException) {
    ForEachType<innerTypes_t>([&]<typename T>() {
        SCOPED_TRACE(std::format("Body type: {}", typeid(T).name()));
        TypeParam message;
        EXPECT_THROW(message.setProtocol(network_n::version_n::Version::NONE), InvalidArgument);
    });
}