#pragma once

#include <format>
#include <HttpCommon.h>
#include <HttpSettings.h>
#include <string>


class HttpHeaders {
public:
    HttpHeaders(std::string RawHeaders) { Parse(RawHeaders); };
    HttpHeaders(std::string Method, std::string Path, const Http_n::Endpoint& Endpoint, HeadersUMap_t HeadersMap) 
        : m_Endpoint(Endpoint), m_Method(Method), m_Path(Path), m_HeadersMap(HeadersMap) {
        Build(Method, Path, Endpoint, HeadersMap);
    };
    
    std::string AsString() { return m_Headers; };
    Http_n::Endpoint Endpoint() { return m_Endpoint; };
    HeadersUMap_t Map() { return m_HeadersMap; };
    std::string Method() { return m_Method; };
    std::string Path() { return m_Path; };
    Http_n::Status Status() { return m_Status; };
    std::string Raw() { return m_RawHeaders; };
    std::string TransferEncoding() { return m_HeadersMap.contains(Http_n::TRANSFER_ENCODING) ? m_HeadersMap[Http_n::TRANSFER_ENCODING] : ""; };
private:
    Http_n::Endpoint m_Endpoint;
    std::string m_Headers;
    HeadersUMap_t m_HeadersMap;
    std::string m_Method;
    std::string m_Path;
    std::string m_RawHeaders;
    Http_n::Status m_Status;

    void Build(std::string Method, std::string Path, const Http_n::Endpoint& Endpoint, HeadersUMap_t HeadersMap) {
        std::string Headers = std::format("{} {} {}\r\nHost: {}", Method, Path, Http_n::PROTOCOL, Endpoint.Host);

        for (auto& [NextHeader, Value] : HeadersMap)
            Headers = std::format("{}\r\n{}: {}\r\n", Headers, NextHeader, Value);

        m_Headers = Headers;
    };

    void Parse(std::string RawHeaders) {
        m_RawHeaders = RawHeaders;
        m_Headers = RawHeaders;

        const std::string ReturnToken = "\r\n";
        std::string Headers = RawHeaders;
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

            m_HeadersMap[Trim(Header)] = Trim(Value);
        }
    };
};