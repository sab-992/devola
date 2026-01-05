#pragma once

#include <memory>
#include <Net.h>
#include <string>
#include <WebsocketBody.h>
#include <WebsocketHeaders.h>


namespace WS_n
{
    template<typename T>
    class Response_i : public Net_n::Response_i, public Net_n::Message_i<T> {
    public:
        virtual std::string APIEndpoint() const = 0;
    };

    template<typename T>
    class Response : public Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>, public WS_n::Response_i<T> {
    public:
        ~Response() {}

        static std::unique_ptr<WS_n::Response_i<T>> Create(std::string RawResponse) {
            return std::unique_ptr<WS_n::Response<T>>(new WS_n::Response<T>(RawResponse));
        }

        template<typename U = T>
        static std::unique_ptr<WS_n::Response_i<T>> Create(Net_n::Code StatusCode, const HeadersUMap_t& HeadersMap, U&& Body = T{}) {
            return std::unique_ptr<WS_n::Response<T>>(new WS_n::Response<T>(StatusCode, HeadersMap, Body));
        }

        std::string APIEndpoint() const override { return this->m_Headers.APIEndpoint(); }

        T Body() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Body(); }

        std::string GetHeader(std::string Header) const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::GetHeader(Header); }

        std::string Headers() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::HeadersMap(); }

        Net_n::Status Status() const override { return this->m_Headers.Status(); }

    protected:
        std::string ToString() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::ToString(); }

    private:
        Response(std::string Response) {
            std::pair<std::string, std::string> SplitMessage = this->Split(Response);
            this->Initialize(WS_n::Headers(SplitMessage.first), WS_n::Body<T>::Parse(SplitMessage.second));
        }

        Response(Net_n::Code StatusCode, const HeadersUMap_t& HeadersMap, T Body) {
            this->Initialize(WS_n::Headers(StatusCode, HeadersMap), WS_n::Body<T>::Build(Body));
        }
    };
}
