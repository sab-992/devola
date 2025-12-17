#pragma once

#include <HttpBody.h>
#include <Net.h>
#include <HttpHeaders.h>
#include <HttpMessage.h>
#include <memory>
#include <string>

namespace Http_n
{
    template<typename T>
    class Response_i : public Net_n::Message_i<T> {};

    template<typename T>
    class Response : public Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>, public Http_n::Response_i<T> {
    public:
        static std::unique_ptr<Http_n::Response_i<T>> Create(std::string RawResponse) {
            return std::unique_ptr<Http_n::Response<T>>(new Http_n::Response<T>(RawResponse));
        }

        static std::unique_ptr<Http_n::Response_i<T>> Create(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, T Body) {
            return std::unique_ptr<Http_n::Response<T>>(new Http_n::Response<T>(StatusCode, HeadersMap, Body));
        }

        std::string APIEndpoint() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::APIEndpoint(); }

        T Body() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Body(); }

        std::string GetHeader(std::string Header) const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::GetHeader(Header); }

        std::string Headers() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::HeadersMap(); }

        std::string Method() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Method(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::NetworkEndpoint(); }

        std::string ToString() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::ToString(); }

        Net_n::Status Status() const override { return Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Status(); }

    private:
        Response(std::string Response)
        : Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>(Response) {}

        Response(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, T Body)
        : Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>::Message_c(StatusCode, HeadersMap, Body) {}
    };
}
