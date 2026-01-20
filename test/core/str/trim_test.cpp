#include <gtest/gtest.h>
#include <trim.h>
#include <string>

class TrimTest : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(TrimTest, TrimStrings) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(trim(input), expected);
}

INSTANTIATE_TEST_SUITE_P(TrimTestSuite,
                         TrimTest,
                         ::testing::Values(std::make_pair("T", "T"),
                                           std::make_pair("    T T T T ", "T T T T"),
                                           std::make_pair(" T T T T    ", "T T T T"),
                                           std::make_pair("T T T T    ", "T T T T"),
                                           std::make_pair("    T T T T", "T T T T"),
                                           std::make_pair("\t\n T \t\n", "T"),
                                           std::make_pair("", ""),
                                           std::make_pair("   ", "")));