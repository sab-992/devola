#pragma once

#include <core/network/interface/headers.h>
#include <core/network/network.h>
#include <core/str/string_formattable.h>
#include <string>
#include <unordered_map>


namespace network_n
{
    class Headers : public network_n::Headers_i {
    public:
        ~Headers() = default;

        std::string apiEndpoint() const override { return m_apiEndpoint; }

        std::string get() const override { return m_headers; }

        std::string getHeader(std::string header) const override { return m_headersMap.contains(header) ? m_headersMap.at(header) : ""; }

        HeadersUMap_t map() const override { return m_headersMap; }

        network_n::Status status() const override { return m_status; }

    protected:
        std::string m_apiEndpoint;
        std::string m_headers;
        HeadersUMap_t m_headersMap;
        network_n::Status m_status;

        Headers() {}

        Headers(std::string headers) : m_headers(headers) {}

        Headers(std::string apiEndpoint, const HeadersUMap_t& headersMap) : m_apiEndpoint(apiEndpoint), m_headersMap(headersMap) {}

        Headers(network_n::Code statusCode, const HeadersUMap_t& headersMap) : m_status(network_n::Status(statusCode)), m_headersMap(headersMap) {}
    
        void parse(std::string rawHeaders) {
            if (rawHeaders.empty())
                return;


            std::string headers = extractMessageInformation(rawHeaders);

            const std::string returnToken = "\r\n";          
            size_t endOfLine = headers.find(returnToken);
            while(endOfLine != std::string::npos) {
                if (trim(headers).empty())
                    break;

                const std::string line = headers.substr(0, endOfLine);
                size_t startOfNextLine = endOfLine + returnToken.size();
                if (startOfNextLine >= headers.size() and endOfLine < headers.size())
                    startOfNextLine = endOfLine;

                headers = headers.substr(startOfNextLine);
                endOfLine = headers.find(returnToken);

                const size_t valueStartPosition = line.find(':');
                if (valueStartPosition == std::string::npos)
                    continue;

                const std::string header = line.substr(0, valueStartPosition);
                const std::string value = line.substr(valueStartPosition + 1);

                m_headersMap[trim(header)] = trim(value);
            }
        }

        std::string toString() const override { return m_headers; }
    };
}