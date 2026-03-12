#pragma once

#include <core/network/interface/protocol.h>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <utility>


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

            std::pair<Headers, Body<T>> parse(std::string raw) const override {
                std::string rawHeaders;
                std::string rawBody;

                Headers headers = Headers();

                // TODO, parse the raw request and fill headers and rawBody variables

                return std::make_pair(headers, Body<T>(preprocessBody(BodyPreprocessor::mergeBody, headers, rawBody)));;
            }

            std::string toString() const override { return "HTTP/1.1"; }

        private:
            const uint16_t DEFAULT_PORT = 80;
            const std::string CHUNKED = "chunked";
            const std::string TRANSFER_ENCODING = "Transfer-Encoding";

            bool isBodyChunked(const Headers& headers) const { return headers.getHeader(TRANSFER_ENCODING) == CHUNKED; }

            std::string preprocessBody(const std::function<std::string(std::string)>& operation, const Headers& headers, std::string stringBody) const {
                return isBodyChunked(headers) ? operation(stringBody) : stringBody;
            }

            class BodyPreprocessor {
            public:
                BodyPreprocessor() = delete;

                static std::string mergeBody(std::string chunkedBody) { /* TODO */ return ""; }
                static std::string chunkBody(std::string mergedBody) { /* TODO */ return ""; }
            };
        };
    }
}