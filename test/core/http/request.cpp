#include <gtest/gtest.h>

#include <core/http/request.h>
#include <format>


class RequestTests : public ::testing::Test {
protected:
    template<typename T>
    http_n::Request<T> createDummyRequest() {
        return std::move(http_n::Request<T>().setMethod("GET")
                                             .setAPIEndpoint("/")
                                             .setURL("www.test.com"));
    }
};

TEST_F(RequestTests, Host) {
    using namespace http_n;
    Request<std::string> request = createDummyRequest<std::string>();

    const std::string EXPECTED_URL = "www.test.com";
    const uint16_t EXPECTED_PORT = 4992;

    request.setURL(EXPECTED_URL);
    request.setPort(EXPECTED_PORT);
    
    request.build();

    EXPECT_EQ(std::format("{}:{}", EXPECTED_URL, EXPECTED_PORT), request.header("Host"));
}