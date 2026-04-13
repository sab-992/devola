#include <gtest/gtest.h>

#include <core/str/hex.h>
#include <cmath>
#include <string>


#define HEX_TO_LONG_PARAM_TYPES std::pair<std::string, unsigned long>
#define LONG_TO_HEX_PARAM_TYPES std::pair<std::pair<unsigned long, bool>, std::string>

const unsigned long FFFFFF = std::pow(2, 24) - 1;

class HexToLongTest : public ::testing::TestWithParam<HEX_TO_LONG_PARAM_TYPES> {};

TEST_P(HexToLongTest, HexToLong) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(fromHex(input), expected);
}

INSTANTIATE_TEST_SUITE_P(HexTestSuite, HexToLongTest, ::testing::Values(HEX_TO_LONG_PARAM_TYPES{ "270F",   9999UL },
                                                                        HEX_TO_LONG_PARAM_TYPES{ "FFFFFF", FFFFFF },
                                                                        HEX_TO_LONG_PARAM_TYPES{ "000000", 0UL }));

class LongToHex : public ::testing::TestWithParam<LONG_TO_HEX_PARAM_TYPES> {};
TEST_P(LongToHex, LongToHex) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(toHex(input.first, input.second), expected);
}

INSTANTIATE_TEST_SUITE_P(HexTestSuite, LongToHex, ::testing::Values(LONG_TO_HEX_PARAM_TYPES{ { 9999UL,    true }, "270F" },
                                                                    LONG_TO_HEX_PARAM_TYPES{ { FFFFFF,    true }, "FFFFFF" },
                                                                    LONG_TO_HEX_PARAM_TYPES{ { 0UL,       true }, "0" },
                                                                    LONG_TO_HEX_PARAM_TYPES{ { 1291762UL, false }, "13b5f2" }));