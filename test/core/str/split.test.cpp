#include <gtest/gtest.h>

#include <core/str/split.h>
#include <string>
#include <utility>


#define SPLIT_STRING_PARAM std::make_pair<std::pair<std::string, char>, std::vector<std::string>>

class SplitTest : public ::testing::TestWithParam<std::pair<std::pair<std::string, char>, std::vector<std::string>>> {};

TEST_P(SplitTest, TrimStrings) {
    const auto [input, expected] = GetParam();
    std::vector<std::string> result;

    split(input.first, result, input.second);

    EXPECT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(SplitTestSuite,
                         SplitTest,
                         ::testing::Values(SPLIT_STRING_PARAM({ "T1 T2 T3", ' ' },      { "T1", "T2", "T3" }),
                                           SPLIT_STRING_PARAM({ " T1 T2 ", ' ' },       { "", "T1", "T2", "" }),
                                           SPLIT_STRING_PARAM({ "T1T2", ' ' },          { "T1T2" }),
                                           SPLIT_STRING_PARAM({ "", ' ' },              { "" }),
                                           SPLIT_STRING_PARAM({ "   ", ' ' },           { "", "", "", "" }),
                                           SPLIT_STRING_PARAM({ "T1 ;T2; T3 ", ';' },   { "T1 ", "T2", " T3 " })));