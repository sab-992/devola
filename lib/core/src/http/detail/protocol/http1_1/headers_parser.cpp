#include <core/http/detail/version/http1_1/headers_parser.hpp>


http_n::version_n::http1_1_n::HeadersParser::HeadersParser(const Singleton<HeadersParser>::Private_s&) {};

bool http_n::version_n::http1_1_n::HeadersParser::operator==(const HeadersParser_i& other) const {
    return dynamic_cast<const HeadersParser*>(&other) != nullptr;
}

std::string http_n::version_n::http1_1_n::HeadersParser::build(const Headers& headers) const {
    const std::string& startLine = headers.startLine();
    if (startLine.empty()) throw InvalidArgument("Cannot be empty", "Start line");

    std::string stringHeaders = startLine;
    for (const auto& [header, value] : headers.toMap())
        stringHeaders += std::format("\r\n{}: {}", header, value);

    return stringHeaders;
}

bool http_n::version_n::http1_1_n::HeadersParser::isContentChunked(const Headers& headers) {
    return headers.get(TRANSFER_ENCODING) == CHUNKED;
}

bool http_n::version_n::http1_1_n::HeadersParser::isResponse(const startLineInformation_t& information) const {
    return information[0].find(PROTOCOL_VERSION_NAME) != std::string::npos;
}

bool http_n::version_n::http1_1_n::HeadersParser::isRequest(const startLineInformation_t& information) const {
    return information[2].find(PROTOCOL_VERSION_NAME) != std::string::npos;
}

std::pair<std::string, headers_t> http_n::version_n::http1_1_n::HeadersParser::parse(std::string_view stringHeaders) const {
    std::stringstream input;
    input << trim(stringHeaders);

    std::string line;
    std::getline(input, line);

    validateStartline(splitStartLine(line));

    const std::string startLine = line;
    headers_t headersUMap;
    for (; std::getline(input, line);) {
        line = trim(line);

        if (line.empty())
            continue;

        size_t separator = line.find(":");

        if (separator == std::string::npos)
            throw InvalidArgument("Ill-formed", std::format("Header: \"{}\"", line));

        const std::string name = line.substr(0, separator);
        const std::string value = trim(line.substr(separator + 1));

        headersUMap[name] = value;
    }

    return { rTrim(startLine), headersUMap };
}

startLineInformation_t http_n::version_n::http1_1_n::HeadersParser::parseStartLine(std::string_view startLine) const {
    const startLineInformation_t& startLineParts = splitStartLine(startLine);

    validateStartline(startLineParts);

    if (not isRequest(startLineParts) and not isResponse(startLineParts))
        throw InvalidArgument("Ill-formed: Message is neither a response nor a request", "HTTP message startLine");

    return startLineParts;
}

startLineInformation_t http_n::version_n::http1_1_n::HeadersParser::splitStartLine(std::string_view startLine) const {
    uint8_t index = 0;
    startLineInformation_t startLineInformation;
    for (; index < 2; ++index) {
        size_t endOfWord = startLine.find(" ");

        if (endOfWord == std::string::npos)
            break;

        const std::string_view word = startLine.substr(0, endOfWord);
        if (word.empty())
            continue;

        startLineInformation[index] = word;
        startLine = startLine.substr(endOfWord + 1);
    }

    const std::string_view lastWord = startLine.substr(0);
    if (not lastWord.empty())
        startLineInformation[index] = lastWord;

    return startLineInformation;
};

void http_n::version_n::http1_1_n::HeadersParser::validateStartline(const startLineInformation_t& startLineInformation) const {
    for (const auto& elem : startLineInformation)
        if (elem.empty()) throw InvalidArgument("Ill-formed: Missing information", "HTTP message startLine");
};