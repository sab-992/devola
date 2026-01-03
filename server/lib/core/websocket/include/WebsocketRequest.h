#pragma once

#include <memory>
#include <Net.h>
#include <NotImplemented.h>
#include <string>
#include <WebsocketMessage.h>


namespace WS_n
{
    template<typename T>
    class Request_i : public Net_n::Message_i<T> {}; 

    template<typename T>
    class Request : public WS_n::Message_c<T>, public WS_n::Request_i<T> {
    public:
        static std::unique_ptr<WS_n::Request_i<T>> Create(std::string RawRequest) {
            return std::unique_ptr<WS_n::Request<T>>(new WS_n::Request<T>(RawRequest));
        }

        template<typename U = T>
        static std::unique_ptr<WS_n::Request_i<T>> Create(std::string APIEndpoint, HeadersUMap_t HeadersMap, U&& Body = T{}) {
            return std::unique_ptr<WS_n::Request<T>>(new WS_n::Request<T>(APIEndpoint, HeadersMap, std::forward<U>(Body)));
        }

        std::string APIEndpoint() const override { return WS_n::Message_c<T>::APIEndpoint(); }

        T Body() const override { return WS_n::Message_c<T>::Body(); }

        std::string GetHeader(std::string Header) const override { return WS_n::Message_c<T>::GetHeader(Header); }

        std::string Headers() const override { return WS_n::Message_c<T>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return WS_n::Message_c<T>::HeadersMap(); }

        std::string Method() const override { throw Except_n::NotImplemented(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { throw Except_n::NotImplemented(); }

        Net_n::Status Status() const override { throw Except_n::NotImplemented(); }

    protected:
        std::string ToString() const override { return WS_n::Message_c<T>::ToString(); }

    private:
        Request(std::string Request)
        : WS_n::Message_c<T>(Request) {}

        Request(std::string APIEndpoint, HeadersUMap_t HeadersMap, T Body)
        : WS_n::Message_c<T>(APIEndpoint, {}, HeadersMap, Body) {}
    };
}