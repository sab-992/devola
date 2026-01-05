#pragma once

#include <HttpBody.h>
#include <HttpHeaders.h>
#include <memory>
#include <Message.h>
#include <Net.h>
#include <string>

namespace Http_n
{
    template<typename T>
    class Response_i : public Net_n::Response_i, public Net_n::Message_i<T> {};

    template<typename T>
    class Response : public Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>, public Http_n::Response_i<T> {
    public:
        ~Response() {};

        static std::unique_ptr<Http_n::Response_i<T>> Create(std::string RawResponse) {
            return std::unique_ptr<Http_n::Response<T>>(new Http_n::Response<T>(RawResponse));
        }

        template<typename U = T>
        static std::unique_ptr<Http_n::Response_i<T>> Create(Net_n::Code StatusCode, const HeadersUMap_t& HeadersMap, U&& Body = T{}, bool ChunkMessage = false) {
            return std::unique_ptr<Http_n::Response<T>>(new Http_n::Response<T>(StatusCode, HeadersMap, Body, ChunkMessage));
        }

        T Body() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Body(); }

        std::string GetHeader(std::string Header) const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::GetHeader(Header); }

        std::string Headers() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::HeadersMap(); }

        Net_n::Status Status() const override { return this->m_Headers.Status(); }

    protected:
        std::string ToString() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::ToString(); }

    private:
        Response(std::string Response) {
            std::pair<std::string, std::string> SplitMessage = this->Split(Response);
            Http_n::Headers Headers(SplitMessage.first);
            this->Initialize(Headers, Http_n::Body<T>::Parse(SplitMessage.second, Headers.GetHeader(Http_n::TRANSFER_ENCODING)));
        }

        Response(Net_n::Code StatusCode, const HeadersUMap_t& HeadersMap, T Body, bool ChunkMessage) {
            this->Initialize(Http_n::Headers(StatusCode, HeadersMap), Http_n::Body<T>::Build(Body, ChunkMessage));
        }
    };
}