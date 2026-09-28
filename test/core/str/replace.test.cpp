#include <gtest/gtest.h>

#include <core/str/replace.hpp>
#include <string>
#include <tuple>


#define REPLACE_STRING_PARAM_TYPES std::pair<std::tuple<std::string, std::string, std::string, size_t, size_t>, std::string>

class ReplaceTest : public ::testing::TestWithParam<REPLACE_STRING_PARAM_TYPES> {};

TEST_P(ReplaceTest, ReplaceWithValidParameters_ReturnValidStringWithReplacedTokens) {
    const auto& [input, expected] = GetParam();

    const std::string& result = replace(std::get<0>(input), std::get<1>(input), std::get<2>(input), std::get<3>(input), std::get<4>(input));

    EXPECT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(ReplaceTestSuite, ReplaceTest, ::testing::Values(REPLACE_STRING_PARAM_TYPES{ { "T1",              "T",     "B",     0, std::string::npos }, "B1" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "1T",              "T",     "B",     0, std::string::npos }, "1B" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { " T1 T2 ",         " ",     "-",     0, std::string::npos }, "-T1-T2-" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "T1T2",            " ",     "B",     0, std::string::npos }, "T1T2" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "",                " ",     "B",     0, std::string::npos }, "" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "   ",             " ",     "T",     0, std::string::npos }, "TTT" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "T1worldT2worlT3", "world", "hello", 0, std::string::npos }, "T1helloT2worlT3" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "T1teT2tT3",       "te",    "B",     2, std::string::npos }, "T1BT2tT3" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "T1testT2",        "test",  "B",     0, 2 },                 "T1testT2" },
                                                                          REPLACE_STRING_PARAM_TYPES{ { "T1,T2,T3,",        ",",     "B",    2, 6 },                "T1BT2BT3," }));