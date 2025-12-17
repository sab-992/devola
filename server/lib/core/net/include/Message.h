#pragma once

#include <memory>
#include <Net.h>
#include <string>


namespace Net_n
{
    template <typename T, typename U, typename V>
    class Message_c : public Net_n::Message_i<T> {
    public:
        ~Message_c() override {}

        std::string APIEndpoint() const override { return Net_n::Message_c<T, U, V>::m_Headers.APIEndpoint(); }

        T Body() const override { return m_Body.Get(); }

        std::string GetHeader(std::string Header) const override { return m_Headers.GetHeader(Header); }

        std::string Headers() const override { return m_Headers.Get(); }

        HeadersUMap_t HeadersMap() const override { return m_Headers.Map(); }

        std::string Method() const override {  return Net_n::Message_c<T, U, V>::m_Headers.Method(); }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { return m_Headers.NetworkEndpoint(); }
        
        Net_n::Status Status() const override { return m_Headers.Status(); }

        std::string ToString() const override { return std::format("{}\r\n\r\n{}", m_Headers.ToString(), m_Body.ToString()); }

    protected:
        V m_Body;
        U m_Headers;

        Message_c(std::string Message) requires (Net_n::IsHeader_cpt<U> && Net_n::IsBody_cpt<V, T>) {
            std::pair<std::string, std::string> SplitMessage = Net_n::Message_c<T, U, V>::Split(Message);
            m_Headers = U(SplitMessage.first);
            m_Body = V(SplitMessage.second, m_Headers.GetHeader(Http_n::TRANSFER_ENCODING));
        }

        Message_c(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap, T Body) requires (Net_n::IsHeader_cpt<U> && Net_n::IsBody_cpt<V, T>) {
            m_Headers = U(Method, APIEndpoint, NetworkEndpoint, HeadersMap);
            m_Body = V(Body, m_Headers.GetHeader(Http_n::TRANSFER_ENCODING));
        }
    
        Message_c(Net_n::Code StatusCode, HeadersUMap_t HeadersMap, T Body) requires (Net_n::IsHeader_cpt<U> && Net_n::IsBody_cpt<V, T>) {
            m_Headers = U(StatusCode, HeadersMap);
            m_Body = V(Body, m_Headers.GetHeader(Http_n::TRANSFER_ENCODING));
        }

        std::pair<std::string, std::string> Split(std::string Message) {
            const std::string HEADER_END_TOKEN = "\r\n\r\n";
            const size_t END_OF_HEADERS = Message.find(HEADER_END_TOKEN);

            if (END_OF_HEADERS == std::string::npos)
                return { "", "" };

            // Returned pair = { Headers (string), Body (string) }.
            return std::make_pair(Message.substr(0, END_OF_HEADERS + HEADER_END_TOKEN.size()), Message.substr(END_OF_HEADERS  + HEADER_END_TOKEN.size()));
        }
    };
}