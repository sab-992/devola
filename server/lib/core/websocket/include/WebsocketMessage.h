#pragma once

#include <Net.h>
#include <NotImplemented.h>
#include <memory>
#include <string>
#include <WebsocketBody.h>
#include <WebsocketHeaders.h>


namespace WS_n
{
    template<typename T>
    class Message_c : public Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>> {
    public:
        ~Message_c() override {}

        std::string APIEndpoint() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::APIEndpoint(); }

        T Body() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Body(); }

        std::string GetHeader(std::string Header) const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::GetHeader(Header); }

        std::string Headers() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Headers(); }

        HeadersUMap_t HeadersMap() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::HeadersMap(); }

        std::string Method() const override { throw Except_n::NotImplemented(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { throw Except_n::NotImplemented(); }

        Net_n::Status Status() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Status(); }


    protected:
        Message_c(std::string Message) {
            std::pair<std::string, std::string> SplitMessage = Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Split(Message);
            Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Initialize(WS_n::Headers(SplitMessage.first),
                                                                          WS_n::Body<T>::Parse(SplitMessage.second));
        }

        Message_c(std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap, T Body) {
            Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Initialize(WS_n::Headers(APIEndpoint, NetworkEndpoint, HeadersMap),
                                                                          WS_n::Body<T>::Build(Body));
        }

        Message_c(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, T Body) {
            Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::Initialize(WS_n::Headers(StatusCode, HeadersMap),
                                                                          WS_n::Body<T>::Build(Body));
        }

        std::string ToString() const override { return Net_n::Message_c<T, WS_n::Headers, WS_n::Body<T>>::ToString(); }
    };
}