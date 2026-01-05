#pragma once

#include <format>
#include <Headers.h>
#include <Net.h>
#include <HttpSettings.h>
#include <Split.h>
#include <string>


// TODO: Move function implementation to .cpp file
namespace Http_n
{
    class ExtraHeaders_i {
    public:
        virtual std::string Method() const = 0;
    };

    class Headers: public Net_n::Headers_c, public Http_n::ExtraHeaders_i {
    public:
        Headers() {}

        Headers(std::string Headers)
        : Net_n::Headers_c(Headers) { this->Parse(Headers); }

        Headers(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, const HeadersUMap_t& HeadersMap) 
        : Net_n::Headers_c(APIEndpoint, HeadersMap), m_Method(Method) {
            this->m_HeadersMap["Host"] = std::format("{}:{}", NetworkEndpoint.Host(), NetworkEndpoint.Port());
            Build(std::format("{} {} {}", m_Method, m_APIEndpoint, Http_n::PROTOCOL));
        }

        Headers(Net_n::Code StatusCode, const HeadersUMap_t& HeadersMap)
        : Net_n::Headers_c(StatusCode, HeadersMap) { /* TODO: Add a way to build response headers */ }

        std::string Method() const override { return m_Method; }

    private:
        std::string m_Method;

        void Build(std::string Headers) {
            for (auto& [NextHeader, Value] : m_HeadersMap)
                Headers = Headers + std::format("\r\n{}: {}\r\n", NextHeader, Value);

            this->m_Headers = Headers;
        }

        std::string ExtractMessageInformation(std::string Headers) override {
            size_t EndOfLine = Headers.find("\r\n");
            std::string Information = Headers.substr(0, EndOfLine);
            std::vector<std::string> RequestInfoVector;
            Split(Information, RequestInfoVector);

            if (RequestInfoVector.size() != 3) // Never more/less than 3 words on the first line
                return ""; // TODO: Replace this with a throw error

            if (RequestInfoVector[0].find("HTTP") == std::string::npos) {
                m_Method = RequestInfoVector[0];
                m_APIEndpoint = RequestInfoVector[1];
            }
            else
                this->m_Status = { static_cast<Net_n::Code>(std::stoi(RequestInfoVector[1])) };

            return LTrim(Headers).substr(EndOfLine);
        }
    };
}