#include <gtest/gtest.h>

#include <core/conversion/enum.h>
#include <core/exception.h>
#include <core/http/response.h>
#include <core/http/utils/build_headers.h>
#include <core/xml/xml.h>
#include <format>


using http_n::Response;

class ResponseTest : public ::testing::Test {};

TEST_F(ResponseTest, Constructor_HasDefaultProtocol) {
    Response<xml_n::Document> response = Response<xml_n::Document>();
    EXPECT_EQ(http_n::DEFAULT_PROTOCOL, response.protocol());
}

TEST_F(ResponseTest, SetStatus_CreatesStatusWithCorrectCode) {
    using namespace http_n;
    using network_n::Code;

    const Code EXPECTED_CODE = Code::OK;
    Response<std::string> response = Response<std::string>();

    response.setStatus(EXPECTED_CODE);

    EXPECT_EQ(EXPECTED_CODE, response.status().code());
}

TEST_F(ResponseTest, BuildWithStatus_CreatesValidStartline) {
    using nlohmann::json;
    using namespace network_n;
    using namespace network_n::protocol_n;

    const Status_s EXPECTED_STATUS(Code::NOT_FOUND);
    const protocol_n::Protocol EXPECTED_PROTOCOL = protocol_n::Protocol::HTTP1_1;

    const std::string EXPECTED_STARTLINE = std::format("{} {} {}", Factory::create<json>(EXPECTED_PROTOCOL)->name(), to_underlying(EXPECTED_STATUS.code()), EXPECTED_STATUS.reason());

    Response<json> response = Response<json>().setStatus(EXPECTED_STATUS.code()).build();

    const std::string stringResponse = response.toString();
    std::string startLine = stringResponse.substr(0, stringResponse.find("\r\n"));

    EXPECT_EQ(EXPECTED_STATUS.code(), response.status().code());
    EXPECT_EQ(EXPECTED_STARTLINE, startLine);
}

TEST_F(ResponseTest, BuildWithoutStatus_ThrowsException) {
    Response<std::string> response = Response<std::string>();

    EXPECT_THROW(response.build(), InvalidArgument);
}

TEST_F(ResponseTest, BuildWithoutProtocol_ThrowsException) {
    using network_n::Code;
    using network_n::protocol_n::Protocol; 
    Response<std::string> response = Response<std::string>().setStatus(Code::CREATED);

    response.setProtocol(Protocol::NONE);

    EXPECT_THROW(response.build(), InvalidArgument);
}

TEST_F(ResponseTest, Set_CreatesValidHTTPResponse) {
    using namespace http_n;
    using namespace network_n;

    const Status_s EXPECTED_STATUS(Code::NOT_FOUND);
    const protocol_n::Protocol EXPECTED_PROTOCOL = protocol_n::Protocol::HTTP1_1;
    const std::string EXPECTED_BODY = "Hello world!";
    const std::map<std::string, std::string> sortedHeadersMap{ {"Transfer-encoding", "chunked"}, {"Content-Length", std::to_string(EXPECTED_BODY.size())}};

    const std::string EXPECTED_RESPONSE = std::format("{} {} {}\r\n{}\r\n{}", protocol_n::Factory::create<std::string>(EXPECTED_PROTOCOL)->name(),
                                                                              to_underlying(EXPECTED_STATUS.code()),
                                                                              EXPECTED_STATUS.reason(),
                                                                              buildHeaders(sortedHeadersMap),
                                                                              EXPECTED_BODY);

    Response<std::string> response = Response<std::string>().set(EXPECTED_RESPONSE);

    EXPECT_NO_THROW(response.build());

    EXPECT_EQ(EXPECTED_STATUS.code(), response.status().code());
    EXPECT_EQ(EXPECTED_PROTOCOL, response.protocol());
    EXPECT_EQ(sortedHeadersMap.size(), response.headersMap().size());
    for (const auto& [header, expected_value]: sortedHeadersMap)
        EXPECT_EQ(expected_value, response.header(header));
    EXPECT_EQ(EXPECTED_BODY, response.body());
}
