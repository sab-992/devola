#include <gtest/gtest.h>

#include <core/utility/enum.hpp>
#include <core/exception.hpp>
#include <core/http/response.hpp>
#include <core/xml/document.hpp>
#include <format>
#include <helper/core/build_headers.hpp>
#include <helper/core/inner_types.hpp>


using http_n::Response;

template<typename T>
class ResponseTest : public ::testing::Test {
protected:
    auto getTestObject(bool alt=false) {
        return InnerTypes<T>::getTestObject(InnerTypes<T>::getTestString(alt));
    }

    std::string getTestStringObject(bool alt=false) {
        return InnerTypes<T>::getTestStringFromObject(this->getTestObject(alt));
    }
};

TYPED_TEST_SUITE(ResponseTest, innerTypes_t);

TYPED_TEST(ResponseTest, Constructor_HasDefaultProtocol) {
    Response response;

    EXPECT_EQ(network_n::protocol_n::Factory::create(http_n::DEFAULT_PROTOCOL), response.protocol());
}

TYPED_TEST(ResponseTest, SetStatus_CreatesStatusWithCorrectCode) {
    using namespace http_n;
    using network_n::Code;

    Response response;
    const Code EXPECTED_CODE = Code::OK;

    response.setStatus(EXPECTED_CODE);

    EXPECT_EQ(EXPECTED_CODE, response.status().code());
}

TYPED_TEST(ResponseTest, BuildWithStatus_CreatesValidStartline) {
    using nlohmann::json;
    using namespace network_n;
    using namespace http_n::protocol_n;
    using namespace network_n::protocol_n;

    const Status_s EXPECTED_STATUS(Code::NOT_FOUND);
    const protocol_n::Protocol EXPECTED_PROTOCOL = protocol_n::Protocol::HTTP1_1;
    const std::string EXPECTED_STARTLINE = std::format("{} {} {}", Factory::create(EXPECTED_PROTOCOL)->name(), to_underlying(EXPECTED_STATUS.code()), EXPECTED_STATUS.reason());

    Response response;
    response.setStatus(EXPECTED_STATUS.code()).build();

    const std::string stringResponse = response.toString();
    const std::string startLine = stringResponse.substr(0, stringResponse.find("\r\n"));

    EXPECT_EQ(EXPECTED_STARTLINE, startLine);
}

TYPED_TEST(ResponseTest, PrepareTransmissionPackets_ReturnsPacketsToSend) {
    using namespace http_n;
    using network_n::Code;

    const std::vector<std::string> EXPECTED_PACKETS = { "HTTP/1.1 200 OK\r\nAccept: text/html", this->getTestStringObject() };
    Response response;
    response.setStatus(Code::OK)
            .setHeader("Accept", "text/html")
            .setBody<TypeParam>(this->getTestObject()).build();

    const auto result = response.prepareTransmissionPackets();

    EXPECT_EQ(EXPECTED_PACKETS, result);
}

TYPED_TEST(ResponseTest, BuildWithoutStatus_ThrowsException) {
    Response response;

    EXPECT_THROW(response.build(), InvalidArgument);
}

TYPED_TEST(ResponseTest, Set_CreatesValidHTTPResponse) {
    using namespace http_n::protocol_n;
    using namespace network_n::protocol_n;

    const network_n::Status_s EXPECTED_STATUS(network_n::Code::NOT_FOUND);
    const Protocol EXPECTED_PROTOCOL = Protocol::HTTP1_1;
    const auto EXPECTED_BODY = this->getTestObject();
    const auto EXPECTED_STRING_BODY = this->getTestStringObject();
    const auto protocol = Factory::create(EXPECTED_PROTOCOL);
    const std::unordered_map<std::string, std::string> headersUMap{ {"Transfer-encoding", "chunked"}, {"Content-Length", std::to_string(EXPECTED_STRING_BODY.size())}};

    const std::string EXPECTED_RESPONSE = std::format("{} {} {}\r\n{}\r\n\r\n{}", protocol->name(),
                                                                                  to_underlying(EXPECTED_STATUS.code()),
                                                                                  EXPECTED_STATUS.reason(),
                                                                                  buildHeaders(headersUMap),
                                                                                  EXPECTED_STRING_BODY);

    Response response;
    response.set(EXPECTED_RESPONSE);

    EXPECT_NO_THROW(response.build());
    EXPECT_EQ(EXPECTED_STATUS, response.status());
    EXPECT_EQ(protocol, response.protocol());
    EXPECT_EQ(headersUMap.size(), response.headersMap().size());
    for (const auto& [header, expected_value]: headersUMap)
        EXPECT_EQ(expected_value, response.header(header));
    EXPECT_EQ(EXPECTED_BODY, response.body<TypeParam>());
}