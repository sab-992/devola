#pragma once

#include <core/exception.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/interface/body_parser.h>
#include <core/network/network.h>
#include <core/str/hex.h>
#include <core/str/trim.h>
#include <core/utility/singleton.h>
#include <string>


namespace network_n
{
    namespace protocol_n
    {
        namespace http1_1_n
        {
            template<typename T>
            class BodyParser : public BodyParser_i<T>, public Singleton<BodyParser<T>> {
            public:
                BodyParser(const Singleton<BodyParser<T>>::Creator_s&) {};

                BodyParser(const BodyParser&) = delete;
                BodyParser& operator=(const BodyParser&) = delete;

                BodyParser(BodyParser&&) = delete;
                BodyParser& operator=(BodyParser&&) = delete;

                ~BodyParser() {}

                bool operator==(const BodyParser_i<T>& other) const override { return dynamic_cast<const BodyParser<T>*>(&other) != nullptr; }

                std::vector<std::string> build(const Headers& headers, const Body<T>& body) const override {
                    if (not HeadersParser::isContentChunked(headers))
                        return { body.toString() };

                    return chunk(headers, body.toString());
                }

                std::string parse(const Headers& headers, std::string_view stringBody) const override {
                    if (not HeadersParser::isContentChunked(headers))
                        return std::string(stringBody);

                    return merge(stringBody);
                }

            private:
                std::vector<std::string> chunk(const Headers& headers, std::string_view mergedBody) const {
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

                size_t getChunkSize(std::string_view message, size_t startPos=0) const {
                    const std::string nextLine = "\r\n";
                    const size_t EOL = message.find(nextLine);

                    if (EOL == std::string::npos)
                        throw InvalidArgument("One or more chunk sizes are ill-formed", "HTTP message body");

                    const std::string_view hexLength = message.substr(startPos, EOL);

                    try { return fromHex(hexLength); } catch (...) {}

                    throw InvalidArgument("Couldn't extract body chunk sizes", "HTTP message body");
                }

                std::string merge(std::string_view rawBody) const {
                    std::string_view chunkedBody = lTrim(rawBody);

                    if (chunkedBody.empty())
                        return "";

                    const std::string nextLine = "\r\n";
                    std::string merged;
                    while (const size_t nextChunkSize = BodyParser<T>::getChunkSize(chunkedBody)) {
                        chunkedBody = chunkedBody.substr(chunkedBody.find(nextLine) + nextLine.size());
                        const std::string_view chunk = chunkedBody.substr(0, nextChunkSize);
                        chunkedBody = chunkedBody.substr(nextChunkSize + 1);
                        merged.append(std::string(chunk));
                    }

                    return rTrim(merged);
                }
            };
        }
    }
}