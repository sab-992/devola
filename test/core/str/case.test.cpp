#include <gtest/gtest.h>

#include <core/str/case.h>
#include <string>


#define CASE_STRING_PARAM_TYPES std::pair<std::string, std::string>

class LowercaseTest : public ::testing::TestWithParam<CASE_STRING_PARAM_TYPES> {};

TEST_P(LowercaseTest, ToLowerWithValidParameters_ReturnsLowercaseString) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(toLower(input), expected);
}

INSTANTIATE_TEST_SUITE_P(LowercaseTestSuite, LowercaseTest, ::testing::Values(CASE_STRING_PARAM_TYPES{ "T",            "t" },
                                                                              CASE_STRING_PARAM_TYPES{ "TttT",         "tttt" },
                                                                              CASE_STRING_PARAM_TYPES{ "tTTt",         "tttt" },
                                                                              CASE_STRING_PARAM_TYPES{ "ttt",          "ttt" },
                                                                              CASE_STRING_PARAM_TYPES{ "\t\n T \t\n",  "\t\n t \t\n" },
                                                                              CASE_STRING_PARAM_TYPES{ "",             "" },
                                                                              CASE_STRING_PARAM_TYPES{ "   ",          "   " }));


class UppercaseTest : public ::testing::TestWithParam<CASE_STRING_PARAM_TYPES> {};

TEST_P(UppercaseTest, ToUpperWithValidParameters_ReturnsUppercaseString) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(toUpper(input), expected);
}

INSTANTIATE_TEST_SUITE_P(UppercaseTestSuite, UppercaseTest, ::testing::Values(CASE_STRING_PARAM_TYPES{ "t",            "T" },
                                                                              CASE_STRING_PARAM_TYPES{ "TttT",         "TTTT" },
                                                                              CASE_STRING_PARAM_TYPES{ "tTTt",         "TTTT" },
                                                                              CASE_STRING_PARAM_TYPES{ "ttt",          "TTT" },
                                                                              CASE_STRING_PARAM_TYPES{ "\t\n t \t\n",  "\t\n T \t\n" },
                                                                              CASE_STRING_PARAM_TYPES{ "",             "" },
                                                                              CASE_STRING_PARAM_TYPES{ "   ",          "   " }));