#pragma once

#include <memory>
#include <Net.h>
#include <NotImplemented.h>
#include <string>
#include <WebsocketMessage.h>


namespace WS_n
{
    template<typename T>
    class Response_i : public Net_n::Message_i<T> {};

    template<typename T>
    class Response : public WS_n::Message_c<T>, public WS_n::Response_i<T> {
    public:
        static std::unique_ptr<WS_n::Response_i<T>> Create(std::string RawResponse) {
            return std::unique_ptr<WS_n::Response<T>>(new WS_n::Response<T>(RawResponse));
        }

        template<typename U = T>
        static std::unique_ptr<WS_n::Response_i<T>> Create(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, U&& Body = T{}) {
            return std::unique_ptr<WS_n::Response<T>>(new WS_n::Response<T>(StatusCode, HeadersMap, Body));
        }

        std::string APIEndpoint() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::APIEndpoint(); }

        T Body() const override { return WS_n::Message_c<T>::Body(); }

        std::string GetHeader(std::string Header) const override { return WS_n::Message_c<T>::GetHeader(Header); }

        std::string Headers() const override { return WS_n::Message_c<T>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return WS_n::Message_c<T>::HeadersMap(); }

        std::string Method() const override { throw Except_n::NotImplemented(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { throw Except_n::NotImplemented(); }

        Net_n::Status Status() const override { return WS_n::Message_c<T>::Status(); }

    protected:
        std::string ToString() const override { return WS_n::Message_c<T>::ToString(); }

    private:
        Response(std::string Response)
        : WS_n::Message_c<T>(Response) {}

        Response(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, T Body)
        : WS_n::Message_c<T>::Message_c(StatusCode, HeadersMap, Body) {}
    };
}
