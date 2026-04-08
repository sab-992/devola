#include <gtest/gtest.h>

#include <core/exception.h>
#include <core/network/detail/body.h>
#include <utils/core/inner_types.h>
#include <sstream>


template<typename T>
class BodyTest : public ::testing::Test {
public:
    template <typename ContentType>
    struct TemplatedBody {
        using type = network_n::Body<ContentType>;
    };

protected:
    auto GetTestBody(bool alt=false) {
        return InnerTypes<T, BodyTest<T>::template TemplatedBody>::GetTestBody(alt);
    }
};

TYPED_TEST_SUITE(BodyTest, networkInnerTypes_t<network_n::Body>);

TYPED_TEST(BodyTest, Constructor_ParsesStringBodyCorrectly) {
    const auto EXPECTED_BODY = this->GetTestBody();
    const auto EXPECTED_STRING_BODY = InnerTypes<TypeParam, BodyTest<TypeParam>::template TemplatedBody>::GetTestStringBody();

    TypeParam body = TypeParam(EXPECTED_STRING_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert());
    EXPECT_EQ(EXPECTED_STRING_BODY, body.toString());
}

TYPED_TEST(BodyTest, Set_AddsNewBody) {
    TypeParam body;
    const auto EXPECTED_BODY = this->GetTestBody();

    body.set(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert());
}

TYPED_TEST(BodyTest, Set_OverwritesExistingBody) {
    TypeParam body;
    const auto ALTERNATE_BODY = this->GetTestBody(true /* alt */);
    const auto EXPECTED_BODY = this->GetTestBody();

    body.set(ALTERNATE_BODY);
    body.set(EXPECTED_BODY);

    EXPECT_EQ(EXPECTED_BODY, body.convert());
}

TYPED_TEST(BodyTest, Convert_ReturnsCorrectBodyType) {
    TypeParam body;
    const auto EXPECTED_BODY = this->GetTestBody();

    body.set(EXPECTED_BODY);

    EXPECT_EQ(typeid(EXPECTED_BODY), typeid(body.convert()));
}

TYPED_TEST(BodyTest, ToString_SerializeBodyCorrectly) {
    TypeParam body;
    const auto BODY = this->GetTestBody();
    std::ostringstream oss;
    oss << BODY;
    const std::string EXPECTED_STRING_BODY = oss.str();

    body.set(BODY);

    EXPECT_EQ(EXPECTED_STRING_BODY, body.toString());
}

TYPED_TEST(BodyTest, SetWithoutAcceptedType_ThrowsException) {
    network_n::Body<int> body;
    EXPECT_THROW(body.set(400), Exception);
}

TYPED_TEST(BodyTest, ConvertWithoutAcceptedType_ThrowsException) {
    network_n::Body<int> body;
    EXPECT_THROW(body.convert(), Exception);
}