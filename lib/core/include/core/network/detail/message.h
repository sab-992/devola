#pragma once

#include <core/conversion/string_convertible.h>
#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <core/network/protocol_factory.h>
#include <core/utility/interface/builder.h>


#define DERIVED_REF_STATIC_CAST static_cast<Derived&>(*this)

namespace network_n
{
    template <typename Derived, typename T>
    class Message : public StringConvertible, public Builder_i<Derived> {
    public:
        virtual ~Message() = default;

        T body() const { return m_body->convert(); }

        Derived& build() & override {
            updateLastBuild();
            finalize();
            return DERIVED_REF_STATIC_CAST;
        }

        Derived build() && override {
            updateLastBuild();
            finalize();
            return std::move(DERIVED_REF_STATIC_CAST);
        }

        std::string header(const std::string& name) const { return m_headers->get(name); }
        headersUMap_t headersMap() const { return m_headers->toMap(); }

        std::vector<std::string> prepareTransmissionPackets() const {
            return getProtocol()->packetize(*this->m_headers, *this->m_body);
        }

        protocol_n::Protocol protocol() {
            return m_protocol;
        }

        Derived& setBody(const T& body) {
            m_body->set(body);
            return DERIVED_REF_STATIC_CAST;
        }

        Derived& setHeader(const std::string& name, const std::string& value) {
            m_headers->setHeader(name, value);
            return DERIVED_REF_STATIC_CAST;
        }

        Derived& setProtocol(protocol_n::Protocol protocol) {
            m_protocol = protocol;
            return DERIVED_REF_STATIC_CAST;
        }

        std::string toString() const override { return getProtocol()->messageToString(*m_headers, *m_body); }

    protected:
        Message(protocol_n::Protocol protocol) : m_protocol(protocol) {
            m_headers = std::make_shared<Headers>(getProtocol()->headersParser());
            m_body = std::make_shared<Body<T>>(getProtocol()->bodyParser());
        }

        Message(const Message&) = default;
        Message& operator=(const Message<Derived, T>&) = default;
        Message(Message&&) = default;

        std::shared_ptr<Body<T>> m_body = nullptr;
        std::shared_ptr<Headers> m_headers = nullptr;
        protocol_n::Protocol m_protocol;

        virtual void updateLastBuild() = 0;
        virtual void finalize() = 0;

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