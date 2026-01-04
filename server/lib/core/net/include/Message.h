#pragma once

#include <concepts>
#include <Converter.h>
#include <Net.h>
#include <string>


namespace Net_n
{
    template <typename T, typename U, typename V>
    class Message_c : public Net_n::Message_i<T> {
    public:
        ~Message_c() override {}

        T Body() const override { return m_Body.Get(); }

        std::string GetHeader(std::string Header) const override { return m_Headers.GetHeader(Header); }

        std::string Headers() const override { return m_Headers.Get(); }

        HeadersUMap_t HeadersMap() const override { return m_Headers.Map(); }

    protected:
        V m_Body;
        U m_Headers;

        void Initialize(U Headers, V Body) requires (Net_n::IsHeader_cpt<U> && Net_n::IsBody_cpt<V, T>) {
            m_Headers = Headers;
            m_Body = Body;
        }

        std::pair<std::string, std::string> Split(std::string Message) {
            const std::string HEADER_END_TOKEN = "\r\n\r\n";
            const size_t END_OF_HEADERS = Message.find(HEADER_END_TOKEN);

            if (END_OF_HEADERS == std::string::npos)
                return { "", "" };

            // Returned pair = { Headers (string), Body (string) }.
            return std::make_pair(Message.substr(0, END_OF_HEADERS + HEADER_END_TOKEN.size()), Message.substr(END_OF_HEADERS  + HEADER_END_TOKEN.size()));
        }

        std::string ToString() const override { return std::format("{}\r\n\r\n{}", Converter<U>::ToString(m_Headers), Converter<V>::ToString(m_Body)); }
    };
}