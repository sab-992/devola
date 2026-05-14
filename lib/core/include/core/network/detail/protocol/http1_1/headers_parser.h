#pragma once

#include <core/exception.h>
#include <core/network/interface/headers_parser.h>
#include <core/network/network.h>
#include <core/str/split.h>
#include <core/str/trim.h>
#include <core/utility/singleton.h>
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
                    const startLineInformation_t& startLineParts = splitStartLine(startLine);

                    validateStartline(startLineParts);

                    if (not isRequest(startLineParts) and not isResponse(startLineParts))
                        throw InvalidArgument("Ill-formed: Message is neither a response nor a request", "HTTP message startLine");

                    return startLineParts;
                }

            private:
                inline static const std::string CHUNKED = "chunked";
                inline static const std::string TRANSFER_ENCODING = "Transfer-Encoding";

                startLineInformation_t splitStartLine(const std::string& startLine) const {
                    std::string startLineCopy(startLine);

                    uint8_t index = 0;
                    startLineInformation_t startLineInformation;
                    for (; index < 2; ++index) {
                        size_t endOfWord = startLineCopy.find(" ");

                        if (endOfWord == std::string::npos)
                            break;

                        const std::string word = startLineCopy.substr(0, endOfWord);
                        if (word.empty())
                            continue;

                        startLineInformation[index] = word;
                        startLineCopy = startLineCopy.substr(endOfWord + 1);
                    }

                    const std::string lastWord = startLineCopy.substr(0);
                    if (not lastWord.empty())
                        startLineInformation[index] = lastWord;

                    return startLineInformation;
                };

                void validateStartline(const startLineInformation_t& startLineInformation) const {
                    for (const auto& elem : startLineInformation)
                        if (elem.empty()) throw InvalidArgument("Ill-formed: Missing information", "HTTP message startLine");
                };

                bool isRequest(const startLineInformation_t& information) const { return information[2].find(PROTOCOL_VERSION_NAME) != std::string::npos; }
                bool isResponse(const startLineInformation_t& information) const { return information[0].find(PROTOCOL_VERSION_NAME) != std::string::npos; }
            };
        }
    }
}