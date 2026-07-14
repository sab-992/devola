#pragma once

#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/network.hpp>
#include <core/network/detail/version/factory.hpp>
#include <core/utility/interface/builder.hpp>
#include <core/utility/string_convertible.hpp>


#define DERIVED_REF_STATIC_CAST static_cast<Derived&>(*this)

namespace network_n
{
    template <typename Derived>
    class Message : public StringConvertible, public Builder_i<Derived> {
        using Version_i = network_n::version_n::Version_i;

    public:
        virtual ~Message() = default;

        template <typename T>
        T body() const { return m_body->convert<T>(); }

        Derived& build() & override {
            finalize();
            updateLastBuild();
            return DERIVED_REF_STATIC_CAST;
        }

        Derived build() && override {
            finalize();
            updateLastBuild();
            return std::move(DERIVED_REF_STATIC_CAST);
        }

        std::string header(const std::string& name) const { return m_headers->get(name); }
        headersUMap_t headersMap() const { return m_headers->toMap(); }

        std::vector<std::string> prepareTransmissionPackets() const {
            assert(not hasChangedSinceLastBuild() && "network_n::Message::build() needs to be called after making changes to the object");
            return version()->packetize(*this->m_headers, *this->m_body);
        }

        std::shared_ptr<Version_i> version() const { return m_version; }

        template <typename T>
        Derived& setBody(const T& body) {
            m_body->set<T>(body);
            return DERIVED_REF_STATIC_CAST;
        }

        Derived& setHeader(const std::string& name, std::string_view value) {
            m_headers->setHeader(name, value);
            return DERIVED_REF_STATIC_CAST;
        }

        Derived& setProtocol(version_n::Version version) {
            return setProtocol(version_n::Factory::create(version));
        }

        Derived& setProtocol(std::shared_ptr<Version_i> version) {
            if (version == nullptr)
                throw InvalidArgument("No version given", "Message version");

            m_version = version;
            return DERIVED_REF_STATIC_CAST;
        }

        std::string toString() const override { return version()->messageToString(*m_headers, *m_body); }

    protected:
        Message(version_n::Version version) {
            setProtocol(version);

            set(Headers(this->version()->headersParser()),
                   Body(this->version()->bodyParser()));
        }

        Message(const Message& other) {
            m_body = other.m_body ? std::make_unique<Body>(*other.m_body) : nullptr;
            m_headers = other.m_headers ? std::make_unique<Headers>(*other.m_headers) : nullptr;
            m_version = other.m_version;
        }

        // Can't use the copy-swap idom because Message cannot be instantiated
        Message& operator=(const Message<Derived>& other) {
            m_body = other.m_body ? std::make_unique<Body>(*other.m_body) : nullptr;
            m_headers = other.m_headers ? std::make_unique<Headers>(*other.m_headers) : nullptr;
            m_version = other.m_version;
        };

        static void swapMessages(Message<Derived>* lhs, Message<Derived>* rhs) {
            using std::swap;

            swap(lhs->m_body, rhs->m_body);
            swap(lhs->m_headers, rhs->m_headers);
            swap(lhs->m_version, rhs->m_version);
        }

        Message& operator=(Message<Derived>&&) = default;
        Message(Message&&) = default;

        std::unique_ptr<Body> m_body;
        std::unique_ptr<Headers> m_headers;
        std::shared_ptr<Version_i> m_version;

        virtual void finalize() = 0;
        virtual bool hasChangedSinceLastBuild() const = 0;
        virtual void updateLastBuild() = 0;

        startLineInformation_t processMessage(std::string_view message) {
            auto [startLineInformation, headers, body] = version()->parse(message);
            set(std::move(headers), std::move(body));
            return startLineInformation;
        }

        void set(network_n::Headers&& headers, network_n::Body&& body) {
            m_headers = std::make_unique<network_n::Headers>(std::move(headers));
            m_body = std::make_unique<network_n::Body>(std::move(body));
        }

        void setStartLine(std::string_view startLine) { m_headers->setStartLine(startLine); }
    };
}