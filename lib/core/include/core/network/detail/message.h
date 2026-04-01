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

        T body() const { return m_body.convert(); }
        std::string header(const std::string& name) const { return m_headers.get(name); }
        headersUMap_t headersMap() const { return m_headers.toMap(); }

        protocol_n::Protocol protocol() {
            return m_protocol;
        }

        Derived& setBody(const T& body) {
            m_body.set(body);
            return static_cast<Derived&>(*this);
        }

        Derived& setHeader(const std::string& name, const std::string& value) {
            m_headers.setHeader(name, value);
            return static_cast<Derived&>(*this);
        }

        Derived& setProtocol(protocol_n::Protocol protocol) {
            m_protocol = protocol;
            return static_cast<Derived&>(*this);
        }

        std::string toString() const override { return getProtocol()->serializeMessage(m_headers, m_body); }

    protected:
        Message() = default;
        Message(const Message&) = default;
        Message& operator=(const Message<Derived, T>&) = default;
        Message(Message&&) = default;

        Body<T> m_body;
        Headers m_headers;
        protocol_n::Protocol m_protocol = protocol_n::Protocol::NONE;

        std::unique_ptr<protocol_n::Protocol_i<T>> getProtocol() const {
            return protocol_n::Factory::create<T>(m_protocol);
        }

        void setStartLine(const std::string& startLine) { m_headers.setStartLine(startLine); }
    };
}