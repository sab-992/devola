#pragma once

#include <core/exception.hpp>
#include <core/network/cookie.hpp>
#include <core/network/interface/headers_parser.hpp>
#include <core/network/network.hpp>
#include <core/str/trim.hpp>
#include <core/utility/compare.hpp>
#include <core/utility/string_convertible.hpp>
#include <string>


using cookies_t = std::unordered_map<std::string, network_n::Cookie>;

namespace network_n
{
    namespace version_n { class HeadersParser_i; }

    class Headers : public StringConvertible {
        using HeadersParser_i = network_n::version_n::HeadersParser_i;

    public:
        Headers(std::shared_ptr<HeadersParser_i> parser);
        Headers(const Headers& other) = default;
        Headers(Headers&& other) = default;

        ~Headers() = default;

        Headers& operator=(Headers other);
        Headers& operator=(Headers&& other) = default;

        bool operator==(const Headers& other) const;

        std::string build() const; // To match the structure of network_n::Body

        const Cookie& cookie(const std::string& name) const;
        std::string get(const std::string& name) const;
        std::string startLine() const;

        cookies_t addedCookiesToMap() const;
        cookies_t cookiesToMap() const;
        headers_t headersToMap() const;
        std::string toString() const override;

        void parse(std::string_view stringHeaders);

        void setCookie(const std::string& name, Cookie value);
        void setHeader(const std::string& name, std::string_view value);
        void setParser(std::shared_ptr<HeadersParser_i> parser);
        void setStartLine(std::string_view startLine);

        friend void swap(Headers& lhs, Headers& rhs) {
            using std::swap;

            swap(lhs.m_cookies, rhs.m_cookies);
            swap(lhs.m_headersMap, rhs.m_headersMap);
            swap(lhs.m_parser, rhs.m_parser);
            swap(lhs.m_setCookies, rhs.m_setCookies);
            swap(lhs.m_startLine, rhs.m_startLine);
        }

    private:
        cookies_t m_cookies;
        headers_t m_headersMap;
        std::shared_ptr<HeadersParser_i> m_parser;
        cookies_t m_setCookies;
        std::string m_startLine;
    };
}