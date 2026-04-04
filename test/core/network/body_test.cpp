#include <gtest/gtest.h>

#include <core/network/detail/body.h>
#include <core/network/utils/inner_types.h>


template<typename T>
class BodyTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(BodyTest);

TYPED_TEST_P(BodyTest, Set_AddsNewBody) {}

REGISTER_TYPED_TEST_SUITE_P(BodyTest, Set_AddsNewBody);

INSTANTIATE_TYPED_TEST_SUITE_P(NetworkBody, BodyTest, networkInnerTypes_t<network_n::Body>);