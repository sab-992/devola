#pragma once

#include <core/network/detail/message.h>
#include <core/http/detail/settings.h>

namespace http_n {
    template <typename T>
    class Request : public network_n::Message<Request<T>, T> {
    public:
        Request() {
            this->m_protocol = http_n::DEFAULT_PROTOCOL;
            m_port = this->protocol()->defaultPort();
        }

        Request(const Request<T>& other) {
            m_APIEndpoint = other.m_APIEndpoint;
            m_method = other.m_method;
            m_port = other.m_port;
            m_URL = other.m_URL;
        }

        Request(Request<T>&& other) {
            m_APIEndpoint = std::move(other.m_APIEndpoint);
            m_method = std::move(other.m_method);
            m_port = std::move(other.m_port);
            m_URL = std::move(other.m_URL);
        }

        Request<T>& operator=(Request<T> other) {
            swap(*this, other);
            return *this;
        }

        friend void swap(Request<T>& lhs, Request<T>& rhs) {
            using std::swap;

            swap(lhs.m_APIEndpoint, rhs.m_APIEndpoint);
            swap(lhs.m_method, rhs.m_method);
            swap(lhs.m_port, rhs.m_port);
            swap(lhs.m_URL, rhs.m_URL);
        }

        std::string APIEndpoint() { return m_APIEndpoint; }

        Request<T>& build() override {
            validateMembers();
            this->setStartLine(std::format("{} {} {}", m_method, m_APIEndpoint, this->protocol()->toString()));
            this->m_headers.setHeader("Host", std::format("{}:{}", m_URL, m_port));
            return *this;
        }

        std::string method() { return m_method; }

        Request<T>& setAPIEndpoint(std::string endpoint) {
            m_APIEndpoint = endpoint;
            return *this;
        }

        Request<T>& setMethod(std::string method) {
            m_method = method;
            return *this;
        }

        Request<T>& setPort(uint16_t port) {
            m_port = port;
            return *this;
        }

        Request<T>& setURL(std::string url) {
            m_URL = url;
            return *this;
        }

    private:
        std::string m_APIEndpoint;
        std::string m_method;
        uint16_t m_port;
        std::string m_URL;

        void validateMembers() const {
            using network_n::protocol_n::Protocol;

            if (m_APIEndpoint.empty()) throw std::invalid_argument("Request API endpoint cannot be empty");
            if (m_method.empty()) throw std::invalid_argument("Request HTTP method cannot be empty");
            if (m_URL.empty()) throw std::invalid_argument("Request host URL cannot be empty");
            if (this->m_protocol == Protocol::NONE) throw std::invalid_argument("Protocol cannot be NONE");
        }
    };
}