#pragma once

#include <HttpMessage.h>
#include <memory>
#include <Net.h>
#include <NotImplemented.h>
#include <string>

namespace Http_n
{
    template<typename T>
    class Response_i : public Net_n::Message_i<T> {};

    template<typename T>
    class Response : public Http_n::Message_c<T>, public Http_n::Response_i<T> {
    public:
        static std::unique_ptr<Http_n::Response_i<T>> Create(std::string RawResponse) {
            return std::unique_ptr<Http_n::Response<T>>(new Http_n::Response<T>(RawResponse));
        }

        template<typename U = T>
        static std::unique_ptr<Http_n::Response_i<T>> Create(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, U&& Body = T{}, bool ChunkMessage = false) {
            return std::unique_ptr<Http_n::Response<T>>(new Http_n::Response<T>(StatusCode, HeadersMap, Body, ChunkMessage));
        }

        std::string APIEndpoint() const override { throw Except_n::NotImplemented(); }

        T Body() const override { return Http_n::Message_c<T>::Body(); }

        std::string GetHeader(std::string Header) const override { return Http_n::Message_c<T>::GetHeader(Header); }

        std::string Headers() const override { return Http_n::Message_c<T>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Http_n::Message_c<T>::HeadersMap(); }

        std::string Method() const override { throw Except_n::NotImplemented(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { throw Except_n::NotImplemented(); }

        Net_n::Status Status() const override { return Http_n::Message_c<T>::Status(); }

    protected:
        std::string ToString() const override { return Http_n::Message_c<T>::ToString(); }

    private:
        Response(std::string Response)
        : Http_n::Message_c<T>(Response) {}

        Response(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, T Body, bool ChunkMessage)
        : Http_n::Message_c<T>::Message_c(StatusCode, HeadersMap, Body, ChunkMessage) {}
    };
}
