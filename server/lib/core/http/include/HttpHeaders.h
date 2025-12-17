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
    class Headers: public Net_n::Headers_c {
    public:
        Headers() {}

        Headers(std::string Headers)
        : Net_n::Headers_c(Headers) { Parse(Headers); }

        Headers(std::string Method, std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint, HeadersUMap_t HeadersMap) 
        : Net_n::Headers_c(Method, APIEndpoint, NetworkEndpoint, HeadersMap) { Build(); }

        Headers(Net_n::Code StatusCode, HeadersUMap_t HeadersMap)
        : Net_n::Headers_c(StatusCode, HeadersMap) { /* TODO: Add a way to build response headers */ }
    private:
        void Build() {

            std::string Headers = std::format("{} {} {}\r\nHost: {}", m_Method, m_APIEndpoint, Http_n::PROTOCOL, m_NetworkEndpoint.Host());
            for (auto& [NextHeader, Value] : m_HeadersMap)
                Headers = std::format("{}\r\n{}: {}\r\n", Headers, NextHeader, Value);

            Net_n::Headers_c::m_Headers = Headers;
        }

        bool ExtractRequestInfo(std::string RawRequestInfo) {
            bool IsResponse = false;
            std::vector<std::string> RequestInfoVector;
            Split(RawRequestInfo, RequestInfoVector);

            if (RequestInfoVector.size() != 3) // Never more/less than 3 words on the first line
                return false;

            if (RequestInfoVector[0].find("HTTP") != std::string::npos)
                return ExtractInfoForResponse(RequestInfoVector);

            return ExtractInfoForRequest(RequestInfoVector);
        }

        bool ExtractInfoForRequest(const std::vector<std::string>& RequestInfoVector) {
            m_Method = RequestInfoVector[0];
            m_APIEndpoint = RequestInfoVector[1];
            return true;
        }

        bool ExtractInfoForResponse(const std::vector<std::string>& RequestInfoVector) {
            Net_n::Headers_c::m_Status = { static_cast<Net_n::Code>(std::stoi(RequestInfoVector[1])) };
            return true;
        }

        void Parse(std::string RawHeaders) {
            if (RawHeaders.empty())
                return;

            const std::string ReturnToken = "\r\n";
            std::string Headers = RawHeaders;
            size_t EndOfLine = Headers.find(ReturnToken);

            ExtractRequestInfo(Headers.substr(0, EndOfLine));

            Headers = LTrim(Headers).substr(EndOfLine);
            EndOfLine = Headers.find(ReturnToken);
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