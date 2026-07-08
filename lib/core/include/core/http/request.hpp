#pragma once

#include <core/exception.hpp>
#include <core/http/detail/memento/request.hpp>
#include <core/http/detail/settings.hpp>
#include <core/network/detail/message.hpp>
#include <core/network/network.hpp>
#include <memory>


namespace http_n
{
    namespace memento_n { class Request; }

    class Request : public network_n::Message<Request> {
        using Body = network_n::Body;
        using Headers = network_n::Headers;
        template<typename T>
        using Message = network_n::Message<T>;

    public:
        Request();
        Request(Headers headers, Body body);
        Request(const Request& other);
        Request(Request&& other) = default;
        ~Request() = default;

        Request& operator=(Request other);
        Request& operator=(Request&& other) = default;

        std::string APIEndpoint() const;
        std::string method() const;
        uint16_t port() const;
        std::string url() const;

        Request& set(std::string_view stringRequest);
        Request& setAPIEndpoint(std::string_view endpoint);
        Request& setMethod(std::string_view method);
        Request& setPort(uint16_t port);
        Request& setURL(std::string_view url);

        friend void swap(Request& lhs, Request& rhs) {
            using std::swap;

            Message<Request>::swapMessages(&lhs, &rhs);

            swap(lhs.m_APIEndpoint, rhs.m_APIEndpoint);
            swap(lhs.m_lastBuild, rhs.m_lastBuild);
            swap(lhs.m_method, rhs.m_method);
            swap(lhs.m_port, rhs.m_port);
            swap(lhs.m_URL, rhs.m_URL);
        }

    protected:
        bool hasChangedSinceLastBuild() const override;

        void finalize() override;
        void updateLastBuild() override;

    private:
        std::string m_APIEndpoint;
        std::unique_ptr<memento_n::Request> m_lastBuild;
        std::string m_method;
        uint16_t m_port;
        std::string m_URL;

        friend class memento_n::Request;

        void initFromStartLineInformation(const startLineInformation_t& information);

        void parse(std::string_view stringRequest);
        std::pair<std::string_view, uint16_t> parseHostURL(std::string_view host);

        void validateMembers() const;
    };
}