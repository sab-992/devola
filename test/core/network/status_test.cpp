#include <gtest/gtest.h>

#include <core/network/network.h>


class StatusTest : public ::testing::Test {};

TEST_F(StatusTest, Constructor_AddsReason) {
    using namespace network_n;
    const Code EXPECTED_STATUS_CODE = Code::NOT_ALLOWED;

    Status_s status(EXPECTED_STATUS_CODE);

    EXPECT_EQ(EXPECTED_STATUS_CODE, status.code());
    EXPECT_EQ(STATUS_REASONS.at(EXPECTED_STATUS_CODE), status.reason());
}