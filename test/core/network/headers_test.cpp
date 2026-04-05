#include <gtest/gtest.h>

#include <core/exception.h>
#include <core/network/detail/headers.h>


class HeadersTest : public ::testing::Test {};

TEST_F(HeadersTest, Constructor_ParsesStringHeadersCorrectly) { /* TODO */ }

TEST_F(HeadersTest, GetWithExistingHeader_ReturnsCorrectHeaderValue) { /* TODO */ }
TEST_F(HeadersTest, GetWithNonExistingHeader_ReturnsEmptyString) { /* TODO */ }

TEST_F(HeadersTest, SetHeader_AddsNewHeader) { /* TODO */ }
TEST_F(HeadersTest, SetHeader_OverwritesExistingHeader) { /* TODO */ }

TEST_F(HeadersTest, StartLine_ReturnsCorrectStartLine) { /* TODO */ }

TEST_F(HeadersTest, SetStartLine_AddsNewStartLine) { /* TODO */ }
TEST_F(HeadersTest, SetStartLine_OverwritesExistingStartLine) { /* TODO */ }

TEST_F(HeadersTest, ToMap_ReturnsMapContainingAllHeaders) { /* TODO */ }
TEST_F(HeadersTest, ToString_ReturnsValidStringHeaders) { /* TODO */ }