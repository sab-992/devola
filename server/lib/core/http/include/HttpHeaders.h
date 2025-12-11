#pragma once

#include <format>
#include <Headers.h>
#include <NetCommon.h>
#include <HttpSettings.h>
#include <Split.h>
#include <string>


// TODO: Move function implementation to .cpp file
class HttpHeaders: public Headers_c {
public:
    HttpHeaders(std::string RawHeaders) { Parse(RawHeaders); };
    HttpHeaders(std::string Method, std::string Path, const Net_n::Endpoint& Endpoint, HeadersUMap_t HeadersMap) 
        : m_Method(Method), m_Path(Path) {
        Headers_c::m_Endpoint = Endpoint;
        Headers_c::m_HeadersMap = HeadersMap;
        Build(Method, Path, Endpoint, HeadersMap);
    };
    
    std::string Method() { return m_Method; };
    std::string Path() { return m_Path; };
    // TODO change to GetHeader;
    std::string TransferEncoding() { return Headers_c::Headers_c::m_HeadersMap.contains(Http_n::TRANSFER_ENCODING) ? Headers_c::Headers_c::m_HeadersMap[Http_n::TRANSFER_ENCODING] : ""; };
private:
    std::string m_Method;
    std::string m_Path;

    void Build(std::string Method, std::string Path, const Net_n::Endpoint& Endpoint, HeadersUMap_t HeadersMap) {
        Headers_c::m_Endpoint = Endpoint;
        Headers_c::Headers_c::m_HeadersMap = HeadersMap;
        m_Method = Method;
        m_Path = Path;

        std::string Headers = std::format("{} {} {}\r\nHost: {}", Method, Path, Http_n::PROTOCOL, Endpoint.Host);
        for (auto& [NextHeader, Value] : HeadersMap)
            Headers = std::format("{}\r\n{}: {}\r\n", Headers, NextHeader, Value);

        Headers_c::m_Headers = Headers;
    };

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
        m_Path = RequestInfoVector[1];
        return true;
    }

    bool ExtractInfoForResponse(const std::vector<std::string>& RequestInfoVector) {
        Headers_c::m_Status = { std::stoi(RequestInfoVector[1]), RequestInfoVector[2] };
        return true;
    }

    void Parse(std::string RawHeaders) {
        Headers_c::m_Headers = RawHeaders;

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

            Headers_c::Headers_c::m_HeadersMap[Trim(Header)] = Trim(Value);
        }
    };
};