#include <core/network/detail/protocol/http1_1/body_parser.hpp>


network_n::protocol_n::http1_1_n::BodyParser::BodyParser(const Singleton<BodyParser>::Private_s&) {};

bool network_n::protocol_n::http1_1_n::BodyParser::operator==(const BodyParser_i& other) const {
    return dynamic_cast<const BodyParser*>(&other) != nullptr;
}

std::vector<std::string> network_n::protocol_n::http1_1_n::BodyParser::build(const Headers& headers, const Body& body) const {
    if (not HeadersParser::isContentChunked(headers))
        return { body.toString() };

    return chunk(headers, body.toString());
}

std::vector<std::string> network_n::protocol_n::http1_1_n::BodyParser::chunk(const Headers& headers, std::string_view mergedBody) const {
    const size_t chunkSize = headers.get("X-IsDownload") == "true" ? DOWNLOAD_BUFFER_MAX_SIZE : REQUEST_BUFFER_MAX_SIZE;

    std::vector<std::string> chunks;
    size_t startOfChunk = 0;

    auto addCurrentChunk = [&mergedBody, &chunks, &startOfChunk](size_t count=std::string::npos){
        std::string_view chunk = mergedBody.substr(startOfChunk, count);
        chunks.emplace_back(std::format("{}\r\n{}", toHex(chunk.size()), chunk));
    };

    while(startOfChunk + chunkSize < mergedBody.size()) {
        addCurrentChunk(chunkSize);
        startOfChunk += chunkSize;
    }

    addCurrentChunk();
    chunks.emplace_back("0\r\n\r\n");
    return chunks;
}

size_t network_n::protocol_n::http1_1_n::BodyParser::getChunkSize(std::string_view message, size_t startPos) const {
    const size_t EOL = message.find(nextLine);

    if (EOL == std::string::npos)
        throw InvalidArgument("One or more chunk sizes are ill-formed", "HTTP message body");

    const std::string_view hexLength = message.substr(startPos, EOL);

    try { return fromHex(hexLength); } catch (...) {}

    throw InvalidArgument("Couldn't extract body chunk sizes", "HTTP message body");
}

std::string network_n::protocol_n::http1_1_n::BodyParser::merge(std::string_view rawBody) const {
    std::string_view chunkedBody = lTrim(rawBody);

    if (chunkedBody.empty())
        return "";

    std::string merged;
    while (const size_t nextChunkSize = BodyParser::getChunkSize(chunkedBody)) {
        chunkedBody = chunkedBody.substr(chunkedBody.find(nextLine) + nextLine.size());
        const std::string_view chunk = chunkedBody.substr(0, nextChunkSize);
        chunkedBody = chunkedBody.substr(nextChunkSize + 1);
        merged.append(std::string(chunk));
    }

    return rTrim(merged);
}

std::string network_n::protocol_n::http1_1_n::BodyParser::parse(const Headers& headers, std::string_view stringBody) const {
    if (not HeadersParser::isContentChunked(headers))
        return std::string(stringBody);

    return merge(stringBody);
}