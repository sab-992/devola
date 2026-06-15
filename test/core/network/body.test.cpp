#include <gtest/gtest.h>

#include <core/exception.hpp>
#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/str/hex.hpp>
#include <sstream>
#include <utils/core/body_parser_mock.hpp>
#include <utils/core/headers_parser_mock.hpp>
#include <utils/core/inner_types.hpp>
#include <vector>


template<typename T>
class BodyTest : public ::testing::Test {
public:
    template <typename ContentType>
    struct TemplatedBody {
        using type = network_n::Body<ContentType>;
    };

    using Inner = InnerTemplatedTypes<T, BodyTest<T>::template TemplatedBody>;

protected:
    auto getTestBody(bool alt=false) {
        return Inner::getTestObject(getTestStringBody(alt));
    }

    std::string getTestStringBody(bool alt=false) {
        return Inner::getTestStringObject(alt);
    }

    auto getParserMock(bool alt=false) {
        return Inner::getBodyParserMock(alt);
    }
};

TYPED_TEST_SUITE(BodyTest, networkTemplatedInnerTypes_t<network_n::Body>);

TYPED_TEST(BodyTest, ConstructorWithNullptr_ThrowsException) {
    EXPECT_THROW(TypeParam(nullptr), InvalidArgument);
}

TYPED_TEST(BodyTest, SetParserWithNullptr_ThrowsException) {
    EXPECT_THROW(TypeParam(this->getParserMock()).setParser(nullptr), InvalidArgument);
}

TYPED_TEST(BodyTest, Build_ReturnsChunkedBodyVector) {
    using namespace network_n;

    std::vector<std::string> EXPECTED_RESULT { this->getTestStringBody() };
    Headers headers(HeadersParserMock::get(false));
    headers.parse("test");
    TypeParam body(this->getParserMock());

    body.parse(headers, this->getTestStringBody());

    EXPECT_EQ(EXPECTED_RESULT, body.build(headers));
}

TYPED_TEST(BodyTest, Parse_ParsesStringBodyCorrectly) {
    const network_n::Headers headers(HeadersParserMock::get());
    const auto EXPECTED_BODY = this->getTestBody();
    const std::string EXPECTED_STRING_BODY = this->getTestStringBody();

    TypeParam body = TypeParam(this->getParserMock());
    body.parse(headers, EXPECTED_STRING_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert());
    EXPECT_EQ(EXPECTED_STRING_BODY, body.toString());
}

TYPED_TEST(BodyTest, Set_AddsNewBody) {
    TypeParam body = TypeParam(this->getParserMock());
    const auto EXPECTED_BODY = this->getTestBody();

    body.set(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert());
}

TYPED_TEST(BodyTest, Set_OverwritesExistingBody) {
    TypeParam body = TypeParam(this->getParserMock());
    const auto ALTERNATE_BODY = this->getTestBody(true /* alt */);
    const auto EXPECTED_BODY = this->getTestBody();

    body.set(ALTERNATE_BODY);
    body.set(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert());
}

TYPED_TEST(BodyTest, Convert_ReturnsCorrectBodyType) {
    TypeParam body = TypeParam(this->getParserMock());
    const auto EXPECTED_BODY = this->getTestBody();

    body.set(EXPECTED_BODY);

    EXPECT_EQ(typeid(EXPECTED_BODY), typeid(body.convert()));
}

TYPED_TEST(BodyTest, ToString_SerializeBodyCorrectly) {
    TypeParam body = TypeParam(this->getParserMock());
    const auto BODY = this->getTestBody();
    std::ostringstream oss;
    oss << BODY;
    const std::string EXPECTED_STRING_BODY = oss.str();

    body.set(BODY);

    EXPECT_EQ(EXPECTED_STRING_BODY, body.toString());
}

TYPED_TEST(BodyTest, SetWithoutAcceptedType_ThrowsException) {
    network_n::Body<int> body(BodyParserMock<int>::get(""));
    EXPECT_THROW(body.set(400), Exception);
}

TYPED_TEST(BodyTest, ConvertWithoutAcceptedType_ThrowsException) {
    network_n::Body<int> body(BodyParserMock<int>::get(""));
    EXPECT_THROW(body.convert(), Exception);
}