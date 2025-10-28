#pragma once

#include <format>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <utility>
#include <unordered_map>

const std::string HEADER_END_TOKEN = "\r\n\r\n";
using HeadersUMap_t = std::unordered_map<std::string, std::string>;
using json = nlohmann::json;

namespace Http_n {
    struct Endpoint { std::string Host; int Port; };

    struct Status { 
        int Code; 
        std::string Reason; 
        std::string ToString() { return std::format("{} {}", Code, Reason); }; 
    };

    class HttpMessageSplitter {
    public:
        HttpMessageSplitter() = delete;

        static std::pair<std::string, std::string> Split(std::string Message) {
            return std::make_pair(ExtractHeaders(Message), ExtractBody(Message));
        };

    private:
        static std::string ExtractBody(std::string Message) {
            std::size_t BodyStartPosition = Message.find(HEADER_END_TOKEN);

            if (BodyStartPosition == std::string::npos)
                return "";

            std::regex Pattern("\r\n\\d+\r\n");
            std::sregex_iterator Begin(Message.begin(), Message.end(), Pattern);
            std::sregex_iterator End;

            std::size_t LastPosition = std::string::npos;
            for (auto It = Begin; It != End; ++It)
                LastPosition = It->position();

            if (LastPosition == std::string::npos)
                return "";

            return Message.substr(BodyStartPosition, LastPosition - BodyStartPosition);
        };

        static std::string ExtractHeaders(std::string Message) {
            return Message.substr(0, Message.find(HEADER_END_TOKEN));
        };
    };
}