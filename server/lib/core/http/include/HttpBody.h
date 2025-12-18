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

        Body(T Body, bool ChunkMessage) { Build(Body); }

        template <typename U>
        Body(U RawBody, std::string TransferEncoding = "") { Parse(std::string(RawBody), TransferEncoding); }

    private:
        void Parse(std::string RawBody, std::string TransferEncoding) {
            Net_n::Body_c<T>::m_RawBody = RawBody;

            if (RawBody.empty())
                return;

            std::string Body = RawBody;
            if (TransferEncoding == Http_n::CHUNKED)
                Body = ExtractChunkedContent(Body);
            Body = Trim(Body);

            if constexpr (std::is_same_v<T, nlohmann::json>)
                Net_n::Body_c<T>::m_Body = json::parse(Body);
            else Net_n::Body_c<T>::m_Body = Body;
        }

        void Build(json Body) {
            Net_n::Body_c<T>::m_Body = Body;
        }

        std::string ExtractChunkedContent(std::string Message) const {
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

        std::string FuseChunks(std::string Message) const {
            std::regex Pattern("^[0-9A-Fa-f]+\r\n", std::regex::multiline);
            std::sregex_iterator Begin(Message.begin(), Message.end(), Pattern);
            std::sregex_iterator End;

            std::size_t StartOfLastChunk = 0;
            std::string FusedChunks;

            for (auto It = Begin; It != End; ++It) {
                // Add current chunk to the result message.
                FusedChunks.append(Message.substr(StartOfLastChunk, It->position()));
                StartOfLastChunk = It->position() +  It->str().size();
            }

            return FusedChunks;
        }
    };
}