#pragma once

#include <core/http/detail/settings.h>
#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <core/str/split.h>
#include <format>
#include <string>


// TODO: Move function implementation to .cpp file
namespace http_n
{
    class ExtraHeaders_i {
    public:
        virtual std::string method() const = 0;
    };

    class Headers: public network_n::Headers_c, public http_n::ExtraHeaders_i {
    public:
        Headers() {}

        Headers(std::string headers)
        : network_n::Headers_c(headers) { this->parse(headers); }

        Headers(std::string method, std::string apiEndpoint, const network_n::Endpoint& networkEndpoint, const HeadersUMap_t& headersMap) 
        : network_n::Headers_c(apiEndpoint, headersMap), m_method(method) {
            this->m_headersMap["Host"] = std::format("{}:{}", networkEndpoint.host(), networkEndpoint.port());
            build(std::format("{} {} {}", m_method, m_apiEndpoint, http_n::PROTOCOL));
        }

        Headers(network_n::Code statusCode, const HeadersUMap_t& headersMap)
        : network_n::Headers_c(statusCode, headersMap) { /* TODO: Add a way to build response headers */ }

        std::string method() const override { return m_method; }

    private:
        std::string m_method;

        void build(std::string headers) {
            for (auto& [nextHeader, value] : m_headersMap)
                headers = headers + std::format("\r\n{}: {}\r\n", nextHeader, value);

            this->m_headers = headers;
        }

        std::string extractMessageInformation(std::string headers) override {
            size_t endOfLine = headers.find("\r\n");
            std::string information = headers.substr(0, endOfLine);
            std::vector<std::string> requestInfoVector;
            split(information, requestInfoVector);

            if (requestInfoVector.size() != 3) // Never more/less than 3 words on the first line
                return ""; // TODO: Replace this with a throw error

            if (requestInfoVector[0].find("HTTP") == std::string::npos) {
                m_method = requestInfoVector[0];
                m_apiEndpoint = requestInfoVector[1];
            }
            else
                this->m_status = { static_cast<network_n::Code>(std::stoi(requestInfoVector[1])) };

            return lTrim(headers).substr(endOfLine);
        }
    };
}