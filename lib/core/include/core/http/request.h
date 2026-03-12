#pragma once

#include <core/network/detail/message.h>


namespace http_n {
    template <typename T>
    class Request : public network_n::Message<Request<T>, T> {
    public:
        Request() { m_port = this->m_protocol->defaultPort(); }

        std::string APIEndpoint() { return m_APIEndpoint; }

        Request<T>& build() override {
            validateMembers();
            this->setStartLine(std::format("{} {} {}", m_method, m_APIEndpoint, this->m_protocol->toString()));
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

        void validateMembers() {
            if (m_APIEndpoint.empty())  throw std::invalid_argument("Request API endpoint cannot be empty");
            if (m_method.empty())       throw std::invalid_argument("Request HTTP method cannot be empty");
            if (m_URL.empty())          throw std::invalid_argument("Request host URL cannot be empty");
        }
    };
}