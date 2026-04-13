#include <gtest/gtest.h>

#include <core/str/trim.h>
#include <string>


#define TRIM_STRING_PARAM_TYPES std::pair<std::string, std::string>

class TrimTest : public ::testing::TestWithParam<TRIM_STRING_PARAM_TYPES> {};

TEST_P(TrimTest, TrimStrings) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(trim(input), expected);
}

INSTANTIATE_TEST_SUITE_P(TrimTestSuite, TrimTest, ::testing::Values(TRIM_STRING_PARAM_TYPES{ "T",            "T" },
                                                                    TRIM_STRING_PARAM_TYPES{ "    T T T T ", "T T T T" },
                                                                    TRIM_STRING_PARAM_TYPES{ " T T T T    ", "T T T T" },
                                                                    TRIM_STRING_PARAM_TYPES{ "T T T T    ",  "T T T T" },
                                                                    TRIM_STRING_PARAM_TYPES{ "    T T T T",  "T T T T" },
                                                                    TRIM_STRING_PARAM_TYPES{ "\t\n T \t\n",  "T" },
                                                                    TRIM_STRING_PARAM_TYPES{ "",             "" },
                                                                    TRIM_STRING_PARAM_TYPES{ "   ",          "" }));