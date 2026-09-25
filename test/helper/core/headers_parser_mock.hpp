#pragma once

#include <core/network/detail/headers.hpp>
#include <core/network/interface/headers_parser.hpp>
#include <core/network/network.hpp>
#include <string>


class HeadersParserMock : public network_n::version_n::HeadersParser_i {
    using Cookie = network_n::Cookie;

public:
    static std::shared_ptr<HeadersParserMock> get(bool isChunked=false, bool isDownload=false) {
        return std::shared_ptr<HeadersParserMock>(new HeadersParserMock(isChunked, isDownload));
    }

    bool operator==(const network_n::version_n::HeadersParser_i& other) const override { return dynamic_cast<const HeadersParserMock*>(&other) != nullptr; }

    std::string build(const network_n::Headers& headers) const override {
        std::string stringHeaders = "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: application/json\r\n"
                                    "Content-Length: 256\r\n"
                                    "Connection: keep-alive\r\n"
                                    "Server: Test/2.4.41";

        if (m_chunked)
            stringHeaders += "\r\nTransfer-Encoding: chunked";

        if (m_isDownload)
            stringHeaders += "\r\nX-IsDownload: true";

        return stringHeaders;
    }

    std::tuple<std::string, std::unordered_map<std::string, std::string>, cookies_t> parse(std::string_view stringHeaders) const override {
        const std::string startLine = "HTTP/1.1 200 OK";
        std::unordered_map<std::string, std::string> headersUMap = { {"Content-Type",   "application/json"},
                                                                     {"Content-Length", "256"},
                                                                     {"Connection",     "keep-alive"},
                                                                     {"Server",         "Test/2.4.41"} };

        if (m_chunked)
            headersUMap.insert({ "Transfer-Encoding", "chunked" });

        if (m_isDownload)
            headersUMap.insert({ "X-IsDownload", "true" });

        const std::string cookieName = "test-cookie";
        return { startLine , headersUMap, {{ cookieName, Cookie().setName(cookieName)
                                                                 .setValue("this is a cookie").build() }}};
    }

    startLineInformation_t parseStartLine(std::string_view startLine) const override { return { "HTTP/1.1", "200", "OK" }; }

private:
    HeadersParserMock(bool isChunked, bool isDownload) : m_chunked(isChunked), m_isDownload(isDownload) {}

    bool m_chunked;
    bool m_isDownload;
};