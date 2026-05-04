#pragma once

#include <core/exception.h>
#include <core/network/interface/headers_parser.h>
#include <core/str/split.h>
#include <core/str/trim.h>
#include <core/utility/interface/singleton.h>
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
                HeadersParser(const Singleton<HeadersParser>::Creator_s&) {};

                HeadersParser(const HeadersParser&) = delete;
                HeadersParser& operator=(const HeadersParser&) = delete;

                HeadersParser(HeadersParser&&) = delete;
                HeadersParser& operator=(HeadersParser&&) = delete;

                ~HeadersParser() {}

                bool operator==(const HeadersParser_i& other) const override { return dynamic_cast<const HeadersParser*>(&other) != nullptr; }

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

                    validateStartline(splitStartLine(line));

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

                    return { rTrim(startLine), headersUMap };
                }

                startLineInformation_t parseStartLine(const std::string& startLine) const override {
                    const std::vector<std::string>& startLineParts = splitStartLine(startLine);

                    validateStartline(startLineParts);

                    if (isRequest(startLineParts))
                        return requestInformationMap(startLineParts);
                    else if (isResponse(startLineParts))
                        return responseInformationMap(startLineParts);

                    throw InvalidArgument("Ill-formed: Message is neither a response nor a request", "HTTP message startLine");
                }

            private:

                inline static const std::string CHUNKED = "chunked";
                inline static const std::string TRANSFER_ENCODING = "Transfer-Encoding";

                std::vector<std::string> splitStartLine(const std::string& startLine) const {
                    std::string startLineCopy(startLine);

                    std::vector<std::string> startLineInformation;
                    while (startLineInformation.size() < 2) {
                        size_t endOfWord = startLineCopy.find(" ");

                        if (endOfWord == std::string::npos)
                            break;

                        const std::string word = startLineCopy.substr(0, endOfWord);
                        startLineInformation.emplace_back(word);
                        startLineCopy = startLineCopy.substr(endOfWord + 1);
                    }

                    const std::string lastWord = startLineCopy.substr(0);
                    if (not lastWord.empty())
                        startLineInformation.emplace_back(lastWord);

                    return startLineInformation;
                };

                void validateStartline(const std::vector<std::string>& startLineInformation) const {
                    // Cannot be more than 3 based on how the startLine is parsed.
                    if (startLineInformation.size() < 3)
                        throw InvalidArgument("Ill-formed: Missing information", "HTTP message startLine");
                };

                bool isRequest(const std::vector<std::string>& information) const { return information[2].find(PROTOCOL_VERSION_NAME) != std::string::npos; }
                bool isResponse(const std::vector<std::string>& information) const { return information[0].find(PROTOCOL_VERSION_NAME) != std::string::npos; }

                startLineInformation_t requestInformationMap(const std::vector<std::string>& information) const {
                    return { { "method", information[0] }, { "APIEndpoint", information[1] }, { "protocol", information[2] } };
                }

                startLineInformation_t responseInformationMap(const std::vector<std::string>& information) const {
                    return { { "protocol", information[0] }, { "code", information[1] }, { "reason", information[2] } };
                }
            };
        }
    }
}