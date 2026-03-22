#pragma once

#include <core/network/interface/protocol.h>
#include <core/str/split.h>
#include <core/str/hex.h>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <regex>


namespace network_n
{
    namespace protocol_n
    {
        template<typename T>
        class HTTP1_1 : public Protocol_i<T> {
        public:
            HTTP1_1() {}

            friend std::unique_ptr<HTTP1_1> std::make_unique<HTTP1_1>();
            
            std::string build(const Headers& headers, const Body<T>& body) const override {
                return std::format("{}\r\n\r\n{}", headers.toString(), preprocessBody(BodyPreprocessor::chunkBody, headers, body.toString()));
            }

            uint16_t defaultPort() const override { return DEFAULT_PORT; }

            std::string name() const override { return "HTTP/1.1"; }

            std::pair<Headers, Body<T>> parse(const std::string& raw) const override {
                auto [rawHeaders, rawBody] = splitMessage(raw);
                Headers headers = Headers(rawHeaders);
                return std::make_pair(headers, Body<T>(preprocessBody(BodyPreprocessor::mergeBody, headers, rawBody)));
            }

            std::unordered_map<std::string, std::string> parseStartLine(const std::string& startLine) const override {
                std::vector<std::string> messageInformation;
                split(startLine, messageInformation);

                if (messageInformation.size() < 3)
                    throw std::invalid_argument("HTTP message is ill-formed");

                if (isRequest(messageInformation))
                    return requestInformationMap(messageInformation);
                else
                    return responseInformationMap(messageInformation);
            }

            std::string serializeMessage(const Headers& headers, const Body<T>& body) const override {
                return std::format("{}\r\n\r\n{}", headers.toString(), body.toString());
            }

        private:
            const uint16_t DEFAULT_PORT = 80;
            const std::string CHUNKED = "chunked";
            const std::string TRANSFER_ENCODING = "Transfer-Encoding";

            bool isBodyChunked(const Headers& headers) const { return headers.get(TRANSFER_ENCODING) == CHUNKED; }

            bool isRequest(const std::vector<std::string>& information) const {
                return information[0].find(name()) == std::string::npos;
            }

            std::string preprocessBody(const std::function<std::string(std::string)>& operation, const Headers& headers, const std::string& stringBody) const {
                return isBodyChunked(headers) ? operation(stringBody) : stringBody;
            }

            std::unordered_map<std::string, std::string> requestInformationMap(const std::vector<std::string>& information) const {
                return { { "method", information[0] }, { "APIEndpoint", information[1] }, { "protocol", information[2] } };
            }

            std::unordered_map<std::string, std::string> responseInformationMap(const std::vector<std::string>& information) const {
                return { { "protocol", information[0] }, { "code", information[1] }, { "reason", information[2] } };
            }

            class BodyPreprocessor {
            public:
                BodyPreprocessor() = delete;

                static std::string mergeBody(const std::string& rawBody) {
                    std::string chunkedBody = lTrim(rawBody);

                    if (chunkedBody.empty())
                        return "";

                    const std::string nextLine = "\r\n";
                    std::string merged;
                    while (const size_t nextChunkSize = BodyPreprocessor::getChunkSize(chunkedBody)) {
                        chunkedBody = chunkedBody.substr(chunkedBody.find(nextLine) + nextLine.size());
                        merged.append(extractChunk(chunkedBody, nextChunkSize));
                    }

                    return rTrim(merged);
                }

                static std::string chunkBody(std::string mergedBody) { /* TODO */ return ""; }

            private:
                static std::string extractChunk(std::string& message, size_t size) {
                    const std::string chunk = message.substr(0, size);
                    message = message.substr(size + 1);
                    return chunk;
                }

                static size_t getChunkSize(const std::string& message) {
                    const std::string nextLine = "\r\n";
                    const size_t EOL = message.find(nextLine);

                    if (EOL == std::string::npos)
                        throw std::invalid_argument("Message not chunked correctly");

                    const std::string hexLength = message.substr(0, EOL);
                    return fromHex(hexLength);
                }
            };

            std::pair<std::string, std::string> splitMessage(std::string message) const {
                const std::string HEADER_END_TOKEN = "\r\n\r\n";
                const size_t END_OF_HEADERS = message.find(HEADER_END_TOKEN);

                if (END_OF_HEADERS == std::string::npos)
                    throw std::invalid_argument("HTTP message is ill-formed");

                // Returned pair = { Headers (string), Body (string) }.
                return std::make_pair(message.substr(0, END_OF_HEADERS + HEADER_END_TOKEN.size()), message.substr(END_OF_HEADERS  + HEADER_END_TOKEN.size()));
            }
        };
    }
}