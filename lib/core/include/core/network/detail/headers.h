#pragma once

#include <core/conversion/string_convertible.h>
#include <core/exception.h>
#include <core/network/interface/headers_parser.h>
#include <core/str/trim.h>
#include <core/utility/compare.h>
#include <format>
#include <string>
#include <sstream>
#include <unordered_map>


using headersUMap_t = std::unordered_map<std::string, std::string>;

namespace network_n
{
    namespace protocol_n { class HeadersParser_i; }

    class Headers : public StringConvertible {
    public:
        Headers(std::shared_ptr<protocol_n::HeadersParser_i> parser);
        Headers(const Headers& other);
        Headers(Headers&& other);

        ~Headers();

        Headers& operator=(Headers other);

        bool operator==(const Headers& other) const;

        std::string build() const; // To match the structure of network_n::Body

        std::string get(const std::string& name) const;

        void parse(const std::string& stringHeaders);

        void setHeader(const std::string& name, const std::string& value);
        void setParser(std::shared_ptr<protocol_n::HeadersParser_i> parser);
        void setStartLine(const std::string& startLine);

        std::string startLine() const;

        friend void swap(Headers& lhs, Headers& rhs) {
            using std::swap;

            swap(lhs.m_headersMap, rhs.m_headersMap);
            swap(lhs.m_parser, rhs.m_parser);
            swap(lhs.m_startLine, rhs.m_startLine);
        }

        headersUMap_t toMap() const;
        std::string toString() const override;

    private:
        headersUMap_t m_headersMap;
        std::shared_ptr<protocol_n::HeadersParser_i> m_parser;
        std::string m_startLine;
    };
}