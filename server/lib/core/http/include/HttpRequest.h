#pragma once

#include <HttpMessage.h>
#include <memory>
#include <Net.h>
#include <string>

namespace Http_n
{
    template<typename T>
    class Request_i : public Net_n::Message_i<T> {}; 

    template<typename T>
    class Request : public Http_n::Message_c<T>, public Http_n::Request_i<T> {
    public:
        static std::unique_ptr<Http_n::Request_i<T>> Create(std::string RawRequest) {
            return std::unique_ptr<Http_n::Request<T>>(new Http_n::Request<T>(RawRequest));
        }

        template<typename U = T>
        static std::unique_ptr<Http_n::Request_i<T>> Create(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap, U&& Body = T{}, bool ChunkMessage = false) {
            return std::unique_ptr<Http_n::Request<T>>(new Http_n::Request<T>(Method, APIEndpoint, NetworkEndpoint, HeadersMap, std::forward<U>(Body), ChunkMessage));
        }

        std::string APIEndpoint() const override { return Http_n::Message_c<T>::APIEndpoint(); }

        T Body() const override { return Http_n::Message_c<T>::Body(); }

        std::string GetHeader(std::string Header) const override { return Http_n::Message_c<T>::GetHeader(Header); }

        std::string Headers() const override { return Http_n::Message_c<T>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Http_n::Message_c<T>::HeadersMap(); }

        std::string Method() const override { return Http_n::Message_c<T>::Method(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { return Http_n::Message_c<T>::NetworkEndpoint(); }

        Net_n::Status Status() const override { return Http_n::Message_c<T>::Status(); }

    protected:
        std::string ToString() const override { return Http_n::Message_c<T>::ToString(); }

    private:
        Request(std::string Request)
        : Http_n::Message_c<T>(Request) {}

        Request(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap, T Body, bool ChunkMessage)
        : Http_n::Message_c<T>(Method, APIEndpoint, NetworkEndpoint, HeadersMap, Body, ChunkMessage) {}
    };
}