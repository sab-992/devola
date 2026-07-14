#pragma once

#include <core/exception.hpp>
#include <core/network/interface/headers_parser.hpp>
#include <core/network/network.hpp>
#include <core/str/trim.hpp>
#include <core/utility/compare.hpp>
#include <core/utility/string_convertible.hpp>
#include <string>


namespace network_n
{
    namespace protocol_n { class HeadersParser_i; }

    class Headers : public StringConvertible {
        using HeadersParser_i = network_n::protocol_n::HeadersParser_i;

    public:
        Headers(std::shared_ptr<HeadersParser_i> parser);
        Headers(const Headers& other) = default;
        Headers(Headers&& other) = default;

        ~Headers() = default;

        Headers& operator=(Headers other);
        Headers& operator=(Headers&& other) = default;

        bool operator==(const Headers& other) const;

        std::string build() const; // To match the structure of network_n::Body

        std::string get(const std::string& name) const;

        void parse(std::string_view stringHeaders);

        void setHeader(const std::string& name, std::string_view value);
        void setParser(std::shared_ptr<HeadersParser_i> parser);
        void setStartLine(std::string_view startLine);

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
        std::shared_ptr<HeadersParser_i> m_parser;
        std::string m_startLine;
    };
}