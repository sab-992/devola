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
    class Request_i : public Net_n::Request_i, public Net_n::Message_i<T> {
    public:
        virtual std::string Method() const = 0;
    }; 

    template<typename T>
    class Request : public Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>, public Http_n::Request_i<T> {
    public:
        ~Request() {}

        static std::unique_ptr<Http_n::Request_i<T>> Create(std::string RawRequest) {
            return std::unique_ptr<Http_n::Request<T>>(new Http_n::Request<T>(RawRequest));
        }

        template<typename U = T>
        static std::unique_ptr<Http_n::Request_i<T>> Create(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, const HeadersUMap_t& HeadersMap, U&& Body = T{}, bool ChunkMessage = false) {
            return std::unique_ptr<Http_n::Request<T>>(new Http_n::Request<T>(Method, APIEndpoint, NetworkEndpoint, HeadersMap, std::forward<U>(Body), ChunkMessage));
        }

        std::string APIEndpoint() const override { return this->m_Headers.APIEndpoint(); }

        T Body() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Body(); }

        std::string GetHeader(std::string Header) const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::GetHeader(Header); }

        std::string Headers() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::HeadersMap(); }

        std::string Method() const override { return this->m_Headers.Method(); }

    protected:
        std::string ToString() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::ToString(); }

    private:
        Request(std::string Request) {
            std::pair<std::string, std::string> SplitMessage = this->Split(Request);
            Http_n::Headers Headers(SplitMessage.first);
            this->Initialize(Headers, Http_n::Body<T>::Parse(SplitMessage.second, Headers.GetHeader(Http_n::TRANSFER_ENCODING)));
        }

        Request(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, const HeadersUMap_t& HeadersMap, T Body, bool ChunkMessage) {
            this->Initialize(Http_n::Headers(Method, APIEndpoint, NetworkEndpoint, HeadersMap), Http_n::Body<T>::Build(Body, ChunkMessage));
        }
    };
}