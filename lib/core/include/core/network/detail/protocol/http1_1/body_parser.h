#pragma once

#include <core/exception.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/interface/body_parser.h>
#include <core/network/network.h>
#include <core/str/hex.h>
#include <core/str/trim.h>
#include <mutex>
#include <string>


namespace network_n
{
    namespace protocol_n
    {    
        namespace http1_1_n
        {
            template<typename T>
            class BodyParser : public BodyParser_i<T> {
            public:
                BodyParser(const BodyParser&) = delete;
                BodyParser& operator=(const BodyParser&) = delete;

                BodyParser(BodyParser&&) = delete;
                BodyParser& operator=(BodyParser&&) = delete;

                ~BodyParser() {}

                std::vector<std::string> build(const Headers& headers, const Body<T>& body) const override {
                    if (not HeadersParser::isContentChunked(headers))
                        return { body.toString() };

                    return chunk(headers, body.toString());
                }

                static std::shared_ptr<BodyParser> instance() {
                    std::call_once(m_bodyParserInitFlag, &BodyParser::createInstance);
                    return m_instance;
                }

                std::string parse(const Headers& headers, const std::string& stringBody) const override {
                    if (not HeadersParser::isContentChunked(headers))
                        return stringBody;

                    return merge(stringBody);
                }

            private:
                BodyParser() {};

                inline static std::once_flag m_bodyParserInitFlag;
                inline static std::shared_ptr<BodyParser> m_instance;

                static void createInstance() { m_instance = std::shared_ptr<BodyParser>(new BodyParser()); }
                
                std::vector<std::string> chunk(const Headers& headers, const std::string& mergedBody) const {
                    const size_t chunkSize = headers.get("X-IsDownload") == "true" ? DOWNLOAD_BUFFER_MAX_SIZE : REQUEST_BUFFER_MAX_SIZE;

                    if (mergedBody.size() < chunkSize)
                        return { mergedBody };

                    std::vector<std::string> chunks;
                    size_t startOfChunk = 0;

                    auto addCurrentChunk = [&mergedBody, &chunks, &startOfChunk](size_t count=std::string::npos){
                        const std::string chunk = mergedBody.substr(startOfChunk, count);
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

                std::string extractChunk(std::string& message, size_t size) const {
                    const std::string chunk = message.substr(0, size);
                    message = message.substr(size + 1);
                    return chunk;
                }

                size_t getChunkSize(const std::string& message) const {
                    const std::string nextLine = "\r\n";
                    const size_t EOL = message.find(nextLine);

                    if (EOL == std::string::npos)
                        throw InvalidArgument("Not chunked correctly", "HTTP message body");

                    const std::string hexLength = message.substr(0, EOL);
                    return fromHex(hexLength);
                }

                std::string merge(const std::string& rawBody) const {
                    std::string chunkedBody = lTrim(rawBody);

                    if (chunkedBody.empty())
                        return "";

                    const std::string nextLine = "\r\n";
                    std::string merged;
                    while (const size_t nextChunkSize = BodyParser<T>::getChunkSize(chunkedBody)) {
                        chunkedBody = chunkedBody.substr(chunkedBody.find(nextLine) + nextLine.size());
                        merged.append(extractChunk(chunkedBody, nextChunkSize));
                    }

                    return rTrim(merged);
                }
            };
        }
    }
}