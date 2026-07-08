#pragma once

#include <core/exception.hpp>
#include <core/network/interface/headers_parser.hpp>
#include <core/network/network.hpp>
#include <core/str/split.hpp>
#include <core/str/trim.hpp>
#include <core/utility/singleton.hpp>
#include <format>
#include <string>
#include <utility>


namespace network_n
{
    namespace protocol_n
    {
        namespace http1_1_n
        {
            const std::string PROTOCOL_VERSION_NAME = "HTTP/1.1";

            class HeadersParser : public HeadersParser_i, public Singleton<HeadersParser> {
            public:
                HeadersParser(const Singleton<HeadersParser>::Private_s&);
                ~HeadersParser() = default;

                bool operator==(const HeadersParser_i& other) const override;

                std::string build(const Headers& headers) const override;
                std::pair<std::string, headersUMap_t> parse(std::string_view stringHeaders) const override;
                startLineInformation_t parseStartLine(std::string_view startLine) const override;

                static bool isContentChunked(const Headers& headers);

            private:
                inline static const std::string CHUNKED = "chunked";
                inline static const std::string TRANSFER_ENCODING = "Transfer-Encoding";

                startLineInformation_t splitStartLine(std::string_view startLine) const;
                void validateStartline(const startLineInformation_t& startLineInformation) const;

                bool isRequest(const startLineInformation_t& information) const;
                bool isResponse(const startLineInformation_t& information) const;
            };
        }
    }
}