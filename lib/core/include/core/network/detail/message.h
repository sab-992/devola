#pragma once

#include <core/conversion/string_convertible.h>
#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <core/network/protocol_factory.h>
#include <core/utils/interface/builder.h>


namespace network_n
{
    template <typename Derived, typename T>
    class Message : public StringConvertible, public Builder_i<Derived> {
    public:
        virtual ~Message() = default;

        T getBody() const { return m_body.convert(); }

        std::string getHeader(std::string name) const { return m_headers.getHeader(); }

        headersUMap_t getHeadersAsMap() const { return m_headers.toMap(); }

        Derived& setBody(const T& body) {
            m_body.set(body);
            return static_cast<Derived&>(*this);
        }

        Derived& setHeader(std::string name, std::string value) {
            m_headers.setHeader(name, value);
            return static_cast<Derived&>(*this);
        }

        Derived& setProtocol(Protocol protocol) {
            m_protocol = protocol_n::Factory::get<T>(protocol);
            return static_cast<Derived&>(*this);
        }

        std::string toString() const override { return m_protocol->build(m_headers, m_body); }

    protected:
        Message() {}

        Body<T> m_body;
        Headers m_headers;
        std::unique_ptr<network_n::protocol_n::Protocol_i<T>> m_protocol = protocol_n::Factory::get<T>(DEFAULT_PROTOCOL);

        void setStartLine(std::string startLine) { m_headers.setStartLine(startLine); }
    };
}