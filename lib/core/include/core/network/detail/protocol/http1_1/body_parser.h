#pragma once

#include <core/exception.h>
#include <core/network/detail/protocol/http1_1/headers_parser.h>
#include <core/network/interface/body_parser.h>
#include <core/network/network.h>
#include <core/str/hex.h>
#include <core/str/trim.h>
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
                BodyParser() {}
                ~BodyParser() {}

                std::vector<std::string> build(const Headers& headers, const Body<T>& body) const override {
                    if (not HeadersParser::isContentChunked(headers))
                        return { body.toString() };

                    return chunk(headers, body.toString());
                }

                std::string parse(const Headers& headers, const std::string& stringBody) const override {
                    if (not HeadersParser::isContentChunked(headers))
                        return stringBody;

                    return merge(stringBody);
                }

            private:
                std::vector<std::string> chunk(const Headers& headers, const std::string& mergedBody) const { /* TODO */ return { mergedBody }; }

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