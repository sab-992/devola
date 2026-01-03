#pragma once

#include <format>
#include <Headers.h>
#include <Net.h>
#include <string>

namespace WS_n
{
    class Headers : public Net_n::Headers_c {
    public:
        Headers() {}

        Headers(std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap)
        : Net_n::Headers_c("", APIEndpoint, NetworkEndpoint, HeadersMap) { Build(); };

        Headers(std::string Headers)
        : Net_n::Headers_c(Headers) { Headers_c::Parse(Headers); }

        Headers(Net_n::Code StatusCode, HeadersUMap_t HeadersMap)
        : Net_n::Headers_c(StatusCode, HeadersMap) {};

        void Build() {
            std::string Headers = std::format("Event: {}", m_APIEndpoint);
            for (auto& [NextHeader, Value] : m_HeadersMap)
                Headers = std::format("{}\r\n{}: {}\r\n", Headers, NextHeader, Value);

            Net_n::Headers_c::m_Headers = Headers;
        }

        std::string ExtractMessageInformation(std::string RawHeaders) override { return RawHeaders; }
    };
}