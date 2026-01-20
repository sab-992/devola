#pragma once

#include <core/http/detail/settings.h>
#include <core/network/detail/body.h>
#include <core/network/network.h>
#include <core/str/trim.h>
#include <format>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>


namespace http_n 
{
    template<typename T>
    class Body : public network_n::Body_c<T> {
    public:
        Body() {}

        static http_n::Body<T> parse(std::string rawBody, std::string transferEncoding = "") {
            if (rawBody.empty())
                return http_n::Body<T>();

            std::string body = rawBody;
            if (transferEncoding == http_n::CHUNKED)
                body = extractChunkedContent(body);
            body = trim(body);

            if constexpr (std::is_same_v<T, nlohmann::json>)
                return http_n::Body<T>(json::parse(body));

            return http_n::Body<T>(body);
        }

        static http_n::Body<T> build(T messageBody, bool chunkMessage = false) {
            // TODO: Divide Message into chunks.
            return http_n::Body<T>(messageBody);
        }
    
    private:
        Body(T messageBody) {
            network_n::Body_c<T>::m_body = messageBody;
        }

        static std::string extractChunkedContent(std::string message) {
            if (message.empty())
                return "";

            // Remove the start (Chunk-length) of the first chunk.
            message = lTrim(message);
            const std::string nextLine = "\r\n";
            message = message.substr(message.find(nextLine) + nextLine.size());

            // Find and fuse all the chunks.
            const std::string content = fusedChunks(message);

            // Remove the "0\r\n" marking the end of the transfer.
            return content.substr(0, content.find("0\r\n\r\n"));
        }

        static std::string fusedChunks(const std::string& message) {
            std::regex pattern("(?:^|\r\n)([0-9A-Fa-f]{1,8})\r\n", std::regex::multiline);

            std::string result;
            std::sregex_iterator begin(message.begin(), message.end(), pattern);
            std::sregex_iterator end;

            std::size_t previousEnd = 0;
            for (auto it = begin; it != end; ++it) {
                const auto& match = *it;
                result.append(message.substr(previousEnd, match.position() - previousEnd));
                previousEnd = match.position() + match.length();
            }
            result.append(message.substr(previousEnd));

            return result;
        }
    };
}