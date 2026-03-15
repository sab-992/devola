#pragma once

#include <core/conversion/string_convertible.h>
#include <core/str/trim.h>
#include <format>
#include <string>
#include <unordered_map>


using headersUMap_t = std::unordered_map<std::string, std::string>;

namespace network_n
{
    class Headers : public StringConvertible {
    public:
        Headers();
        Headers(const Headers& other);
        Headers(Headers&& other);

        ~Headers();

        Headers& operator=(Headers other);

        std::string getHeader(std::string name) const;
        std::string getStartLine() const;

        void setHeader(std::string name, std::string value);
        void setStartLine(std::string startLine);

        friend void swap(Headers& lhs, Headers& rhs) {
            using std::swap;

            swap(lhs.m_headersMap, rhs.m_headersMap);
            swap(lhs.m_startLine, rhs.m_startLine);
        }

        headersUMap_t toMap() const;
        std::string toString() const override;

    private:
        headersUMap_t m_headersMap;
        std::string m_startLine;
    };
}