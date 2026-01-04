#pragma once

#include <memory>
#include <Net.h>
#include <string>
#include <WebsocketBody.h>
#include <WebsocketHeaders.h>


namespace WS_n
{
    template<typename T>
    class Request_i : public Net_n::Request_i, public Net_n::Message_i<T> {}; 

    template<typename T>
    class Request : public Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>, public WS_n::Request_i<T> {
    public:
        ~Request() {}

        static std::unique_ptr<WS_n::Request_i<T>> Create(std::string RawRequest) {
            return std::unique_ptr<WS_n::Request<T>>(new WS_n::Request<T>(RawRequest));
        }

        template<typename U = T>
        static std::unique_ptr<WS_n::Request_i<T>> Create(std::string APIEndpoint, HeadersUMap_t HeadersMap, U&& Body = T{}) {
            return std::unique_ptr<WS_n::Request<T>>(new WS_n::Request<T>(APIEndpoint, HeadersMap, std::forward<U>(Body)));
        }

        std::string APIEndpoint() const override { return this->m_Headers.APIEndpoint(); }

        T Body() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Body(); }

        std::string GetHeader(std::string Header) const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::GetHeader(Header); }

        std::string Headers() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::HeadersMap(); }

    protected:
        std::string ToString() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::ToString(); }

    private:
        Request(std::string Request) {
            std::pair<std::string, std::string> SplitMessage = this->Split(Request);
            this->Initialize(WS_n::Headers(SplitMessage.first), WS_n::Body<T>::Parse(SplitMessage.second));
        }

        Request(std::string APIEndpoint, HeadersUMap_t HeadersMap, T Body) {
            this->Initialize(WS_n::Headers(APIEndpoint, HeadersMap), WS_n::Body<T>::Build(Body));
        }
    };
}