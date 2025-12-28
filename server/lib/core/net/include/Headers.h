#pragma once

#include <Net.h>
#include <string>
#include <Str.h>
#include <unordered_map>


namespace Net_n
{
    class Headers_c : public Net_n::Headers_i {
    public:
        ~Headers_c() = default;

        std::string APIEndpoint() const override { return m_APIEndpoint; }

        std::string Get() const override { return m_Headers; }

        std::string GetHeader(std::string Header) const override { return m_HeadersMap.contains(Header) ? m_HeadersMap.at(Header) : ""; }

        HeadersUMap_t Map() const override { return m_HeadersMap; }

        std::string Method() const override { return m_Method; }

        Net_n::NetworkEndpoint NetworkEndpoint() const override { return m_NetworkEndpoint; }

        Net_n::Status Status() const override { return m_Status; }

        std::string ToString() const override { return m_Headers; }

    protected:
        std::string m_APIEndpoint;
        std::string m_Headers;
        HeadersUMap_t m_HeadersMap;
        std::string m_Method;
        Net_n::NetworkEndpoint m_NetworkEndpoint;
        Net_n::Status m_Status;

        Headers_c()
        : m_Method(""), m_APIEndpoint("") {}

        Headers_c(std::string Headers)
        : m_Headers(Headers) {}

        Headers_c(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap)
        : m_Method(Method), m_APIEndpoint(APIEndpoint), m_NetworkEndpoint(NetworkEndpoint), m_HeadersMap(HeadersMap) {}

        Headers_c(Net_n::Code StatusCode, HeadersUMap_t HeadersMap)
        : m_Status(Net_n::Status(StatusCode)), m_HeadersMap(HeadersMap) {}
    
        void Parse(std::string RawHeaders) {
            if (RawHeaders.empty())
                return;


            std::string Headers = ExtractMessageInformation(RawHeaders);

            const std::string ReturnToken = "\r\n";          
            size_t EndOfLine = Headers.find(ReturnToken);
            while(EndOfLine != std::string::npos) {
                if (Trim(Headers).empty())
                    break;

                const std::string Line = Headers.substr(0, EndOfLine);
                size_t StartOfNextLine = EndOfLine + ReturnToken.size();
                if (StartOfNextLine >= Headers.size() and EndOfLine < Headers.size())
                    StartOfNextLine = EndOfLine;

                Headers = Headers.substr(StartOfNextLine);
                EndOfLine = Headers.find(ReturnToken);

                const size_t ValueStartPosition = Line.find(':');
                if (ValueStartPosition == std::string::npos)
                    continue;

                const std::string Header = Line.substr(0, ValueStartPosition);
                const std::string Value = Line.substr(ValueStartPosition + 1);

                Net_n::Headers_c::m_HeadersMap[Trim(Header)] = Trim(Value);
            }
        }
    };
}