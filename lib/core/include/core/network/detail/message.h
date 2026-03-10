#pragma once

#include <core/network/concept/body.h>
#include <core/network/concept/headers.h>
#include <core/network/interface/message.h>
#include <core/network/network.h>
#include <core/utils/converter.h>
#include <string>


namespace network_n
{
    template <typename T, typename U, typename V>
    class Message : virtual public network_n::Message_i<T> {
    public:
        ~Message() override {}

        T body() const override { return m_body.get(); }

        std::string getHeader(std::string header) const override { return m_headers.getHeader(header); }

        std::string headers() const override { return m_headers.get(); }

        HeadersUMap_t headersMap() const override { return m_headers.map(); }

    protected:
        V m_body;
        U m_headers;

        void initialize(U headers, V body) requires (network_n::IsHeader_cpt<U> && network_n::IsBodypt<V, T>) {
            m_headers = headers;
            m_body = body;
        }

        std::pair<std::string, std::string> split(std::string message) {
            const std::string HEADER_END_TOKEN = "\r\n\r\n";
            const size_t END_OF_HEADERS = message.find(HEADER_END_TOKEN);

            if (END_OF_HEADERS == std::string::npos)
                return { "", "" };

            // Returned pair = { Headers (string), Body (string) }.
            return std::make_pair(message.substr(0, END_OF_HEADERS + HEADER_END_TOKEN.size()), message.substr(END_OF_HEADERS  + HEADER_END_TOKEN.size()));
        }

        std::string toString() const override { return std::format("{}\r\n\r\n{}", Converter<U>::toString(m_headers), Converter<V>::toString(m_body)); }
    };
}