#include <gtest/gtest.h>

#include <core/conversion/enum.h>
#include <core/exception.h>
#include <core/http/response.h>
#include <core/xml/xml.h>
#include <format>
#include <utils/core/build_headers.h>
#include <utils/core/inner_types.h>


using http_n::Response;

template<typename T>
class ResponseTest : public ::testing::Test {
public:
    template <typename ContentType>
    struct TemplatedResponse {
        using type = http_n::Response<ContentType>;
    };

    using Inner = InnerTypes<T, ResponseTest<T>::template TemplatedResponse>;

protected:
    auto getTestBody(bool alt=false) {
        return Inner::getTestObject(getTestStringBody(alt));
    }

    std::string getTestStringBody(bool alt=false) {
        return Inner::getTestStringObject(alt);
    }
};

TYPED_TEST_SUITE(ResponseTest, networkInnerTypes_t<http_n::Response>);

TYPED_TEST(ResponseTest, Constructor_HasDefaultProtocol) {
    TypeParam response;

    EXPECT_EQ(network_n::protocol_n::Factory::create<decltype(this->getTestBody())>(http_n::DEFAULT_PROTOCOL), response.protocol());
}

TYPED_TEST(ResponseTest, SetStatus_CreatesStatusWithCorrectCode) {
    using namespace http_n;
    using network_n::Code;

    TypeParam response;
    const Code EXPECTED_CODE = Code::OK;

    response.setStatus(EXPECTED_CODE);

    EXPECT_EQ(EXPECTED_CODE, response.status().code());
}

TYPED_TEST(ResponseTest, BuildWithStatus_CreatesValidStartline) {
    using nlohmann::json;
    using namespace network_n;
    using namespace network_n::protocol_n;

    const Status_s EXPECTED_STATUS(Code::NOT_FOUND);
    const protocol_n::Protocol EXPECTED_PROTOCOL = protocol_n::Protocol::HTTP1_1;
    const std::string EXPECTED_STARTLINE = std::format("{} {} {}", Factory::create<json>(EXPECTED_PROTOCOL)->name(), to_underlying(EXPECTED_STATUS.code()), EXPECTED_STATUS.reason());

    TypeParam response;
    response.setStatus(EXPECTED_STATUS.code()).build();

    const std::string stringResponse = response.toString();
    const std::string startLine = stringResponse.substr(0, stringResponse.find("\r\n"));

    EXPECT_EQ(EXPECTED_STARTLINE, startLine);
}

TYPED_TEST(ResponseTest, BuildWithoutStatus_ThrowsException) {
    TypeParam response;

    EXPECT_THROW(response.build(), InvalidArgument);
}

TYPED_TEST(ResponseTest, Set_CreatesValidHTTPResponse) {
    using namespace http_n;
    using namespace network_n;

    const Status_s EXPECTED_STATUS(Code::NOT_FOUND);
    const protocol_n::Protocol EXPECTED_PROTOCOL = protocol_n::Protocol::HTTP1_1;
    const auto EXPECTED_BODY = this->getTestBody();
    const auto EXPECTED_STRING_BODY = this->getTestStringBody();
    const auto protocol = protocol_n::Factory::create<decltype(this->getTestBody())>(EXPECTED_PROTOCOL);
    const std::unordered_map<std::string, std::string> headersUMap{ {"Transfer-encoding", "chunked"}, {"Content-Length", std::to_string(EXPECTED_STRING_BODY.size())}};

    const std::string EXPECTED_RESPONSE = std::format("{} {} {}\r\n{}\r\n\r\n{}", protocol->name(),
                                                                                  to_underlying(EXPECTED_STATUS.code()),
                                                                                  EXPECTED_STATUS.reason(),
                                                                                  buildHeaders(headersUMap),
                                                                                  EXPECTED_STRING_BODY);

    TypeParam response;
    response.set(EXPECTED_RESPONSE);

    EXPECT_NO_THROW(response.build());
    EXPECT_EQ(EXPECTED_STATUS, response.status());
    EXPECT_EQ(protocol, response.protocol());
    EXPECT_EQ(headersUMap.size(), response.headersMap().size());
    for (const auto& [header, expected_value]: headersUMap)
        EXPECT_EQ(expected_value, response.header(header));
    EXPECT_EQ(EXPECTED_BODY, response.body());
}