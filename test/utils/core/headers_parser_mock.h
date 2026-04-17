#pragma once

#include <core/network/detail/headers.h>
#include <core/network/interface/headers_parser.h>
#include <string>
#include <utility>


class HeadersParserMock : public network_n::protocol_n::HeadersParser_i {
public:
    HeadersParserMock() {}
    ~HeadersParserMock() {}

    friend std::unique_ptr<HeadersParserMock> std::make_unique<HeadersParserMock>();

    std::string build(const network_n::Headers& headers) const override {
        return  "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 256\r\nConnection: keep-alive\r\nServer: Test/2.4.41";
    }

    std::pair<std::string, std::unordered_map<std::string, std::string>> parse(const std::string& stringHeaders) const override {
        const std::string startline = "HTTP/1.1 200 OK";
        const std::unordered_map<std::string, std::string> headersUMap = { {"Content-Type",   "application/json"},
                                                                           {"Content-Length", "256"},
                                                                           {"Connection",     "keep-alive"},
                                                                           {"Server",         "Test/2.4.41"} };
        return { startline , headersUMap };
    }
};