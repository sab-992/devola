#pragma once

#include <core/exception.h>
#include <core/network/interface/headers_parser.h>
#include <core/str/split.h>
#include <core/str/trim.h>
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

            class HeadersParser : public HeadersParser_i {
            public:
                HeadersParser() {}
                ~HeadersParser() {}

                friend std::unique_ptr<HeadersParser> std::make_unique<HeadersParser>();

                std::string build(const Headers& headers) const override {
                    std::string stringHeaders = headers.startLine();

                    for (const auto& [header, value] : headers.toMap())
                        stringHeaders += std::format("\r\n{}: {}", header, value);

                    return stringHeaders;
                }

                static bool isContentChunked(const Headers& headers) { return headers.get(TRANSFER_ENCODING) == CHUNKED; }

                std::pair<std::string, headersUMap_t> parse(const std::string& stringHeaders) const override {
                    std::istringstream input(trim(stringHeaders));
                    std::string line;

                    std::getline(input, line);
                
                    const std::string startLine = line;
                    headersUMap_t headersUMap;
                    for (; std::getline(input, line);) {
                        line = trim(line);
                        size_t separator = line.find(":");

                        if (separator == std::string::npos)
                            throw InvalidArgument("Ill-formed", std::format("Header: \"{}\"", line));

                        const std::string name = line.substr(0, separator);
                        const std::string value = trim(line.substr(separator + 1));

                        headersUMap[name] = value;
                    }

                    return { startLine, headersUMap };
                }

                static std::unordered_map<std::string, std::string> parseStartLine(const std::string& startLine) {
                    std::vector<std::string> messageInformation;
                    split(startLine, messageInformation);

                    if (messageInformation.size() < 3)
                        throw InvalidArgument("Ill-formed: Missing information", "HTTP message startline");

                    if (isRequest(messageInformation))
                        return requestInformationMap(messageInformation);
                    else if (isResponse(messageInformation))
                        return responseInformationMap(messageInformation);

                    throw InvalidArgument("Ill-formed: Message is neither a response nor a request", "HTTP message startline");
                }

            private:
                inline static const std::string CHUNKED = "chunked";
                inline static const std::string TRANSFER_ENCODING = "Transfer-Encoding";

                static bool isRequest(const std::vector<std::string>& information) { return information[2].find(PROTOCOL_VERSION_NAME) != std::string::npos; }
                static bool isResponse(const std::vector<std::string>& information) { return information[0].find(PROTOCOL_VERSION_NAME) != std::string::npos; }

                static std::unordered_map<std::string, std::string> requestInformationMap(const std::vector<std::string>& information) {
                    return { { "method", information[0] }, { "APIEndpoint", information[1] }, { "protocol", information[2] } };
                }

                static std::unordered_map<std::string, std::string> responseInformationMap(const std::vector<std::string>& information) {
                    return { { "protocol", information[0] }, { "code", information[1] }, { "reason", information[2] } };
                }
            };
        }
    }
}