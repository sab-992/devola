#include <gtest/gtest.h>

#include <core/str/split.h>
#include <string>
#include <tuple>


#define SPLIT_STRING_PARAM std::make_pair<std::tuple<std::string, std::string, size_t, size_t>, std::vector<std::string>>

class SplitTest : public ::testing::TestWithParam<std::pair<std::tuple<std::string, std::string, size_t, size_t>, std::vector<std::string>>> {};

TEST_P(SplitTest, TrimStrings) {
    const auto [input, expected] = GetParam();

    std::vector<std::string> result;
    split(std::get<0>(input), result, std::get<1>(input), std::get<2>(input), std::get<3>(input));

    EXPECT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(SplitTestSuite,
                         SplitTest,
                         ::testing::Values(SPLIT_STRING_PARAM({ "T1 T2 T3",        " ",     0, std::string::npos }, { "T1", "T2", "T3" }),
                                           SPLIT_STRING_PARAM({ " T1 T2 ",         " ",     0, std::string::npos }, { "", "T1", "T2", "" }),
                                           SPLIT_STRING_PARAM({ "T1T2",            " ",     0, std::string::npos }, { "T1T2" }),
                                           SPLIT_STRING_PARAM({ "",                " ",     0, std::string::npos }, { "" }),
                                           SPLIT_STRING_PARAM({ "   ",             " ",     0, std::string::npos }, { "", "", "", "" }),
                                           SPLIT_STRING_PARAM({ "T1 ;T2; T3 ",     ";",     0, std::string::npos }, { "T1 ", "T2", " T3 " }),
                                           SPLIT_STRING_PARAM({ "T1worldT2worlT3", "world", 0, std::string::npos }, { "T1", "T2worlT3" }),
                                           SPLIT_STRING_PARAM({ "T1teT2tT3",       "te",    2, std::string::npos }, { "", "T2tT3" }),
                                           SPLIT_STRING_PARAM({ "T1testT2",        "test",  0, 2 },                 { "T1" }),
                                           SPLIT_STRING_PARAM({ "T1,T2,T3",        ",",     2, 6 },                 { "", "T2", "" })));