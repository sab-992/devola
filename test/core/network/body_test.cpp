#include <gtest/gtest.h>

#include <core/network/detail/body.h>
#include <core/network/utils/inner_types.h>


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

TYPED_TEST(BodyTest, Body_IsConvertedCorrectly) {
    TypeParam body;
    const auto EXPECTED_BODY = this->GetTestBody();

    body.set(EXPECTED_BODY);

    EXPECT_EQ(typeid(EXPECTED_BODY), typeid(body.convert()));
}