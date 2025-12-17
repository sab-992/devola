#pragma once

#include <HttpBody.h>
#include <HttpHeaders.h>
#include <HttpMessage.h>
#include <memory>
#include <Net.h>
#include <string>

namespace Http_n
{
    template<typename T>
    class Request_i : public Net_n::Message_i<T> {}; 

    template<typename T>
    class Request : public Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>, public Http_n::Request_i<T> {
    public:
        static std::unique_ptr<Http_n::Request_i<T>> Create(std::string RawRequest) {
            return std::unique_ptr<Http_n::Request<T>>(new Http_n::Request<T>(RawRequest));
        }

        template<typename U = T>
        static std::unique_ptr<Http_n::Request_i<T>> Create(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap, U&& Body = T{}) {
            return std::unique_ptr<Http_n::Request<T>>(new Http_n::Request<T>(Method, APIEndpoint, NetworkEndpoint, HeadersMap, std::forward<U>(Body)));
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
        Request(std::string Request)
        : Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>(Request) {}

        Request(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap, T Body)
        : Net_n::Message_c<T, Http_n::Headers, Http_n::Body<T>>(Method, APIEndpoint, NetworkEndpoint, HeadersMap, Body) {}
    };
}