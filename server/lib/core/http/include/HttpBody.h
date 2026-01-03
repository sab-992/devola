#pragma once

#include <Body.h>
#include <format>
#include <HttpSettings.h>
#include <Net.h>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <Trim.h>

namespace Http_n 
{
    template<typename T>
    class Body : public Net_n::Body_c<T> {
    public:
        Body() {}

        static Http_n::Body<T> Parse(std::string RawBody, std::string TransferEncoding = "") {
            if (RawBody.empty())
                return Http_n::Body<T>();

            std::string Body = RawBody;
            if (TransferEncoding == Http_n::CHUNKED)
                Body = ExtractChunkedContent(Body);
            Body = Trim(Body);

            if constexpr (std::is_same_v<T, nlohmann::json>)
                return Http_n::Body<T>(json::parse(Body));

            return Http_n::Body<T>(Body);
        }

        static Http_n::Body<T> Build(T MessageBody, bool ChunkMessage = false) {
            // TODO: Divide Message into chunks.
            return Http_n::Body<T>(MessageBody);
        }
    
    private:
        Body(T MessageBody) {
            Net_n::Body_c<T>::m_Body = MessageBody;
        }

        static std::string ExtractChunkedContent(std::string Message) {
            if (Message.empty())
                return "";

            // Remove the start (Chunk-length) of the first chunk.
            Message = LTrim(Message);
            const std::string NextLine = "\r\n";
            Message = Message.substr(Message.find(NextLine) + NextLine.size());

            // Find and fuse all the chunks.
            const std::string FusedChunks = FuseChunks(Message);

            // Remove the "0\r\n" marking the end of the transfer.
            return FusedChunks.substr(0, FusedChunks.find("0\r\n\r\n"));
        }

        static std::string FuseChunks(const std::string& Message) {
            std::regex Pattern("(?:^|\r\n)([0-9A-Fa-f]{1,8})\r\n", std::regex::multiline);
            
            std::string Result;
            std::sregex_iterator Begin(Message.begin(), Message.end(), Pattern);
            std::sregex_iterator End;
            
            std::size_t LastEnd = 0;
            
            for (auto It = Begin; It != End; ++It) {
                const auto& Match = *It;
                std::string HexPart = Match[1].str();
                
                Result.append(Message.substr(LastEnd, Match.position() - LastEnd));
                
                LastEnd = Match.position() + Match.length();
            }
            
            Result.append(Message.substr(LastEnd));
            
            return Result;
        }
    };
}