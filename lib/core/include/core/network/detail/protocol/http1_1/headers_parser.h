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
                HeadersParser(const HeadersParser&) = delete;
                HeadersParser& operator=(const HeadersParser&) = delete;

                HeadersParser(HeadersParser&&) = delete;
                HeadersParser& operator=(HeadersParser&&) = delete;

                ~HeadersParser() {}

                std::string build(const Headers& headers) const override {
                    std::string stringHeaders = headers.startLine();

                    for (const auto& [header, value] : headers.toMap())
                        stringHeaders += std::format("\r\n{}: {}", header, value);

                    return stringHeaders;
                }

                static bool isContentChunked(const Headers& headers) { return headers.get(TRANSFER_ENCODING) == CHUNKED; }

                static std::shared_ptr<HeadersParser> instance() {
                    std::call_once(m_headerParserInitFlag, &HeadersParser::createInstance);
                    return m_instance;
                }

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

                static std::unordered_map<std::string, std::string> parseStartLine(const std::string& startLine) {
                    const std::vector<std::string>& messageInformation = splitStartLine(startLine);

                    validateStartline(messageInformation);

                    if (isRequest(messageInformation))
                        return requestInformationMap(messageInformation);
                    else if (isResponse(messageInformation))
                        return responseInformationMap(messageInformation);

                    throw InvalidArgument("Ill-formed: Message is neither a response nor a request", "HTTP message startline");
                }

            private:
                HeadersParser() {};

                inline static const std::string CHUNKED = "chunked";
                inline static const std::string TRANSFER_ENCODING = "Transfer-Encoding";

                inline static std::once_flag m_headerParserInitFlag;
                inline static std::shared_ptr<HeadersParser> m_instance;

                static void createInstance() { m_instance = std::shared_ptr<HeadersParser>(new HeadersParser()); }

                static std::vector<std::string> splitStartLine(const std::string& startLine) {
                    std::string startLineCopy(startLine);

                    std::vector<std::string> messageInformation;
                    while (messageInformation.size() < 2) {
                        size_t endOfWord = startLineCopy.find(" ");

                        if (endOfWord == std::string::npos)
                            break;

                        const std::string word = startLineCopy.substr(0, endOfWord);
                        messageInformation.emplace_back(word);
                        startLineCopy = startLineCopy.substr(endOfWord + 1);
                    }

                    const std::string lastWord = startLineCopy.substr(0);
                    if (not lastWord.empty())
                        messageInformation.emplace_back(lastWord);

                    return messageInformation;
                };

                static void validateStartline(const std::vector<std::string>& messageInformation) {
                    // Cannot be more than 3 based on how the startline is parsed.
                    if (messageInformation.size() < 3)
                        throw InvalidArgument("Ill-formed: Missing information", "HTTP message startline");
                };

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