#pragma once

#include <HttpBody.h>
#include <HttpHeaders.h>
#include <Message.h>
#include <Net.h>
#include <string>


namespace Http_n
{
    template<typename T>
    class Message_c : public Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>> {
    public:
        ~Message_c() override {};

        std::string APIEndpoint() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::APIEndpoint(); }

        T Body() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Body(); }

        std::string GetHeader(std::string Header) const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::GetHeader(Header); }

        std::string Headers() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::HeadersMap(); }

        std::string Method() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Method(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::NetworkEndpoint(); }

        Net_n::Status Status() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Status(); }

        std::string ToString() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::ToString(); }

    protected:
        Message_c(std::string Message) {
            std::pair<std::string, std::string> SplitMessage = Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Split(Message);

            Http_n::Headers Headers(SplitMessage.first);
            Http_n::Body<T> Body(SplitMessage.second, Headers.GetHeader(Http_n::TRANSFER_ENCODING));

            Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Initialize(Headers, Body);
        }

        Message_c(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap, T Body, bool ChunkMessage) {
            Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Initialize(Http_n::Headers(Method, APIEndpoint, NetworkEndpoint, HeadersMap),
                                                                              Http_n::Body<T>(Body, ChunkMessage));
        }

        Message_c(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, T Body, bool ChunkMessage) {
            Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Initialize(Http_n::Headers(StatusCode, HeadersMap), Http_n::Body<T>(Body, ChunkMessage));
        }
    };
}