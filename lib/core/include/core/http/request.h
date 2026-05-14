#pragma once

#include <core/exception.h>
#include <core/http/detail/memento/request.h>
#include <core/http/detail/settings.h>
#include <core/network/detail/message.h>
#include <core/network/network.h>
#include <memory>


namespace http_n
{
    namespace memento_n
    {
        template <typename T>
        class Request;
    }

    template <typename T>
    class Request : public network_n::Message<Request<T>, T> {
    public:
        Request()
        : network_n::Message<Request<T>, T>(http_n::DEFAULT_PROTOCOL), m_port(this->protocol()->defaultPort()) {}

        Request(network_n::Headers headers, network_n::Body<T> body)
        : network_n::Message<Request<T>, T>(http_n::DEFAULT_PROTOCOL), m_port(this->protocol()->defaultPort()) {
            network_n::Message<Request<T>, T>::set(std::move(headers), std::move(body));

            const startLineInformation_t& requestInfo = this->protocol()->headersParser()->parseStartLine(this->m_headers->startLine());
            initFromStartLineInformation(requestInfo);
        }

        Request(network_n::Headers&& headers, network_n::Body<T>&& body)
        : network_n::Message<Request<T>, T>(http_n::DEFAULT_PROTOCOL), m_port(this->protocol()->defaultPort()) {
            network_n::Message<Request<T>, T>::set(headers, body);

            const startLineInformation_t& requestInfo = this->protocol()->headersParser()->parseStartLine(this->m_headers->startLine());
            initFromStartLineInformation(requestInfo);
        }

        Request(const Request<T>& other) : network_n::Message<Request<T>, T>(other) {
            if (other.m_lastBuild)
                m_lastBuild = std::make_unique<memento_n::Request<T>>(*other.m_lastBuild);

            m_APIEndpoint = other.m_APIEndpoint;
            m_method = other.m_method;
            m_port = other.m_port;
            m_URL = other.m_URL;
        }

        Request(Request<T>&& other) : network_n::Message<Request<T>, T>(std::move(other)) {
            m_APIEndpoint = std::move(other.m_APIEndpoint);
            m_lastBuild = std::move(other.m_lastBuild);
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
        std::string method() const { return m_method; }
        uint16_t port() const { return m_port; }

        Request<T>& set(const std::string& stringRequest) {
            parse(stringRequest);
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
            swap(lhs.m_lastBuild, rhs.m_lastBuild);
            swap(lhs.m_method, rhs.m_method);
            swap(lhs.m_port, rhs.m_port);
            swap(lhs.m_URL, rhs.m_URL);
        }

        std::string url() const { return m_URL; }

    protected:
        void finalize() override {
            validateMembers();
            this->setStartLine(std::format("{} {} {}", m_method, m_APIEndpoint, this->protocol()->name()));
            this->m_headers->setHeader("Host", std::format("{}:{}", m_URL, m_port));
        }

        bool hasChangedSinceLastBuild() const override {
            // If m_lastBuild is nullptr, build() was never called and since this
            // is not a static function, we are guaranteed that an object
            // has been created, therefore the request has changed.
            if (not m_lastBuild)
                return true;

            return *m_lastBuild == memento_n::Request<T>(*this);
        }

        void updateLastBuild() override { m_lastBuild = std::make_unique<memento_n::Request<T>>(*this); }

    private:
        std::string m_APIEndpoint;
        std::unique_ptr<memento_n::Request<T>> m_lastBuild;
        std::string m_method;
        uint16_t m_port;
        std::string m_URL;

        friend class memento_n::Request<T>;

        void initFromStartLineInformation(const startLineInformation_t& information) {
            setMethod(information[0]);
            setAPIEndpoint(information[1]);

            const std::string host = this->header("Host");
            if (host.empty()) return;
            const auto& [url, port] = parseHostURL(host);

            setURL(url);
            setPort(port);
        }

        void parse(const std::string& stringRequest) {
            const startLineInformation_t& requestInfo = this->processMessage(stringRequest);
            initFromStartLineInformation(requestInfo);
        }

        std::pair<std::string, uint16_t> parseHostURL(const std::string& host) {
            size_t separatorIndex = host.find(':');

            if (host.empty() or separatorIndex == std::string::npos)
                return { host, m_port };

            return { host.substr(0, separatorIndex), static_cast<uint16_t>(std::stoi(host.substr(separatorIndex + 1))) };
        }

        void validateMembers() const {
            if (m_APIEndpoint.empty()) throw InvalidArgument("Cannot be empty", "API endpoint");
            if (m_method.empty()) throw InvalidArgument("Cannot be empty", "HTTP method");
            if (m_port == 0) throw InvalidArgument("Cannot be '0'", "Host port");
            if (m_URL.empty()) throw InvalidArgument("Cannot be empty", "Host URL");
        }
    };
}