#include <gtest/gtest.h>

#include <core/exception.hpp>
#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/str/hex.hpp>
#include <sstream>
#include <helper/core/body_parser_mock.hpp>
#include <helper/core/headers_parser_mock.hpp>
#include <helper/core/inner_types.hpp>
#include <vector>


using Body = network_n::Body;

template<typename T>
class BodyTest : public ::testing::Test {
protected:
    auto getTestObject(bool alt=false) {
        return InnerTypes<T>::getTestObject(InnerTypes<T>::getTestString(alt));
    }

    std::string getTestStringObject(bool alt=false) {
        return InnerTypes<T>::getTestStringFromObject(this->getTestObject(alt));
    }

    auto getParserMock(bool alt=false) {
        return BodyParserMock::get(getTestStringObject());
    }
};

TYPED_TEST_SUITE(BodyTest, innerTypes_t);

TYPED_TEST(BodyTest, ConstructorWithNullptr_ThrowsException) {
    EXPECT_THROW(Body(nullptr), InvalidArgument);
}

TYPED_TEST(BodyTest, SetParserWithNullptr_ThrowsException) {
    EXPECT_THROW(Body(this->getParserMock()).setParser(nullptr), InvalidArgument);
}

TYPED_TEST(BodyTest, Build_ReturnsChunkedBodyVector) {
    using namespace network_n;

    std::vector<std::string> EXPECTED_RESULT { this->getTestStringObject() };
    Headers headers(HeadersParserMock::get(false));
    headers.parse("test");
    Body body(this->getParserMock());

    body.parse(headers, this->getTestStringObject());

    EXPECT_EQ(EXPECTED_RESULT, body.build(headers));
}

TYPED_TEST(BodyTest, Parse_ParsesStringBodyCorrectly) {
    const network_n::Headers headers(HeadersParserMock::get());
    const auto EXPECTED_BODY = this->getTestObject();
    const std::string EXPECTED_STRING_BODY = this->getTestStringObject();

    Body body = Body(this->getParserMock());
    body.parse(headers, EXPECTED_STRING_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert<TypeParam>());
    EXPECT_EQ(EXPECTED_STRING_BODY, body.toString());
}

TYPED_TEST(BodyTest, Set_AddsNewBody) {
    Body body = Body(this->getParserMock());
    const auto EXPECTED_BODY = this->getTestObject();

    body.set<TypeParam>(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert<TypeParam>());
}

TYPED_TEST(BodyTest, Set_OverwritesExistingBody) {
    Body body = Body(this->getParserMock());
    const auto ALTERNATE_BODY = this->getTestObject(true /* alt */);
    const auto EXPECTED_BODY = this->getTestObject();

    body.set<TypeParam>(ALTERNATE_BODY);
    body.set<TypeParam>(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert<TypeParam>());
}

TYPED_TEST(BodyTest, Convert_ReturnsCorrectBodyType) {
    Body body = Body(this->getParserMock());
    const auto EXPECTED_BODY = this->getTestObject();

    body.set<TypeParam>(EXPECTED_BODY);

    EXPECT_EQ(typeid(EXPECTED_BODY), typeid(body.convert<TypeParam>()));
}

TYPED_TEST(BodyTest, ToString_SerializeBodyCorrectly) {
    Body body = Body(this->getParserMock());
    const auto BODY = this->getTestObject();
    std::ostringstream oss;
    oss << BODY;
    const std::string EXPECTED_STRING_BODY = oss.str();

    body.set<TypeParam>(BODY);

    EXPECT_EQ(EXPECTED_STRING_BODY, body.toString());
}

TYPED_TEST(BodyTest, SetWithoutAcceptedType_ThrowsException) {
    network_n::Body body(BodyParserMock::get(""));
    EXPECT_THROW(body.set<int>(400), Exception);
}

TYPED_TEST(BodyTest, ConvertWithoutAcceptedType_ThrowsException) {
    network_n::Body body(BodyParserMock::get(""));
    EXPECT_THROW(body.convert<int>(), Exception);
}