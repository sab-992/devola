#pragma once

#include <core/conversion/string_convertible.h>
#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <core/network/protocol_factory.h>
#include <core/utility/interface/builder.h>

#include <iostream>


namespace network_n
{
    template <typename Derived, typename T>
    class Message : public StringConvertible, public Builder_i<Derived> {
    public:
        virtual ~Message() = default;

        T body() const { return m_body->convert(); }
        std::string header(const std::string& name) const { return m_headers->get(name); }
        headersUMap_t headersMap() const { return m_headers->toMap(); }

        protocol_n::Protocol protocol() {
            return m_protocol;
        }

        Derived& setBody(const T& body) {
            m_body->set(body);
            return static_cast<Derived&>(*this);
        }

        Derived& setHeader(const std::string& name, const std::string& value) {
            m_headers->setHeader(name, value);
            return static_cast<Derived&>(*this);
        }

        Derived& setProtocol(protocol_n::Protocol protocol) {
            m_protocol = protocol;
            return static_cast<Derived&>(*this);
        }

        std::string toString() const override { return getProtocol()->serializeMessage(*m_headers, *m_body); }

    protected:
        Message(protocol_n::Protocol protocol) : m_protocol(protocol) {
            m_headers = std::make_shared<Headers>(getProtocol()->headersParser());
            m_body = std::make_shared<Body<T>>();
        }

        Message(const Message&) = default;
        Message& operator=(const Message<Derived, T>&) = default;
        Message(Message&&) = default;

        std::shared_ptr<Body<T>> m_body = nullptr;
        std::shared_ptr<Headers> m_headers = nullptr;
        protocol_n::Protocol m_protocol;

        std::unordered_map<std::string, std::string> processMessage(const std::string& message) {
            // TODO: Add protocol detection and change it accordingly
            const auto& [headers, body] = this->getProtocol()->parse(message);

            m_headers = std::make_shared<network_n::Headers>(headers);
            m_body = std::make_shared<network_n::Body<T>>(body);

            return this->getProtocol()->parseStartLine(m_headers->startLine());
        }

        std::unique_ptr<protocol_n::Protocol_i<T>> getProtocol() const {
            return protocol_n::Factory::create<T>(m_protocol);
        }

        void setStartLine(const std::string& startLine) { m_headers->setStartLine(startLine); }
    };
}