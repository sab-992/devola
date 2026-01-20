#pragma once

#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <format>
#include <string>


namespace websocket_n
{
    class ExtraHeaders_i {};

    class Headers : public network_n::Headers_c, public websocket_n::ExtraHeaders_i {
    public:
        Headers() {}

        Headers(std::string apiEndpoint, const HeadersUMap_t& headersMap)
        : network_n::Headers_c(apiEndpoint, headersMap) { build(); };

        Headers(std::string headers)
        : network_n::Headers_c(headers) { this->parse(headers); }

        Headers(network_n::Code statusCode, const HeadersUMap_t& headersMap)
        : network_n::Headers_c(statusCode, headersMap) {};

        void build() {
            std::string headers = std::format("Event: {}", m_apiEndpoint);
            for (auto& [nextHeader, value] : m_headersMap)
                headers = std::format("{}\r\n{}: {}\r\n", headers, nextHeader, value);

            this->m_headers = headers;
        }

        std::string extractMessageInformation(std::string rawHeaders) override { return rawHeaders; }
    };
}