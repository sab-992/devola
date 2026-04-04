#pragma once

#include <core/exception.h>
#include <core/http/detail/settings.h>
#include <core/network/detail/message.h>

namespace http_n {
    template <typename T>
    class Request : public network_n::Message<Request<T>, T> {
    public:
        Request() { this->m_protocol = http_n::DEFAULT_PROTOCOL; }

        Request(const Request<T>& other) : network_n::Message<Request<T>, T>(other) {
            m_APIEndpoint = other.m_APIEndpoint;
            m_method = other.m_method;
            m_port = other.m_port;
            m_URL = other.m_URL;
        }

        Request(Request<T>&& other) : network_n::Message<Request<T>, T>(std::move(other)) {
            m_APIEndpoint = std::move(other.m_APIEndpoint);
            m_method = std::move(other.m_method);
            m_port = std::move(other.m_port);
            m_URL = std::move(other.m_URL);
        }

        ~Request() {}

        Request<T>& operator=(Request<T> other) {
            swap(*this, other);
            return *this;
        }

        std::string APIEndpoint() const { return m_APIEndpoint; }

        Request<T>& build() & override {
            finalize();
            return *this;
        }

        Request<T> build() && override {
            finalize();
            return std::move(*this);
        }

        std::string get() {
            return this->getProtocol()->build(this->m_headers, this->m_body);
        }

        std::string method() const { return m_method; }
        uint16_t port() const { return m_port; }

        Request<T>& set(const std::string& stringRequest) {
            parseFrom(stringRequest);
            return *this;
        }

        Request<T>& setAPIEndpoint(const std::string& endpoint) {
            m_APIEndpoint = endpoint;
            return *this;
        }

        Request<T>& setMethod(const std::string& method) {
            m_method = method;
            return *this;
        }

        Request<T>& setPort(uint16_t port) {
            m_port = port;
            return *this;
        }

        Request<T>& setURL(const std::string& url) {
            m_URL = url;
            return *this;
        }

        friend void swap(Request<T>& lhs, Request<T>& rhs) {
            using std::swap;

            swap(static_cast<network_n::Message<Request<T>, T>&>(lhs), static_cast<network_n::Message<Request<T>, T>&>(rhs));

            swap(lhs.m_APIEndpoint, rhs.m_APIEndpoint);
            swap(lhs.m_method, rhs.m_method);
            swap(lhs.m_port, rhs.m_port);
            swap(lhs.m_URL, rhs.m_URL);
        }

        std::string url() const { return m_URL; }

    private:
        std::string m_APIEndpoint;
        std::string m_method;
        uint16_t m_port = 0;
        std::string m_URL;

        void finalize() {
            validateMembers();
            m_port = m_port == 0 ? this->getProtocol()->defaultPort() :  m_port;
            this->setStartLine(std::format("{} {} {}", m_method, m_APIEndpoint, this->getProtocol()->name()));
            this->m_headers.setHeader("Host", std::format("{}:{}", m_URL, m_port));
        }

        void parseFrom(const std::string& stringRequest) {
            // TODO: Add protocol detection and change it accordingly
            auto [headers, body] = this->getProtocol()->parse(stringRequest);

            this->m_headers = headers;
            this->m_body = body;

            const std::unordered_map<std::string, std::string> requestInfo = this->getProtocol()->parseStartLine(this->m_headers.startLine());

            m_APIEndpoint = requestInfo.at("APIEndpoint");
            m_method = requestInfo.at("method");

            const std::string host = this->header("Host");

            if (host.empty())
                return;

            auto [url, port] = parseHostURL(host);
            m_URL = url;
            m_port = port;
        }

        std::pair<std::string, uint16_t> parseHostURL(const std::string& host) {
            size_t separatorIndex = host.find(':');

            if (host.empty() or separatorIndex == std::string::npos)
                return { host, 0 };

            return { host.substr(0, separatorIndex), static_cast<uint16_t>(std::stoi(host.substr(separatorIndex + 1))) };
        }

        void validateMembers() const {
            using network_n::protocol_n::Protocol;

            if (m_APIEndpoint.empty()) throw InvalidArgument("Cannot be empty", "API endpoint");
            if (m_method.empty()) throw InvalidArgument("Cannot be empty", "HTTP method");
            if (m_URL.empty()) throw InvalidArgument("Cannot be empty", "Host URL");
            if (this->m_protocol == Protocol::NONE) throw InvalidArgument("Cannot be NONE", "Protocol");
        }
    };
}