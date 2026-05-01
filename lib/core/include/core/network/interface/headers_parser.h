#pragma once

#include <core/network/detail/headers.h>
#include <string>
#include <utility>


namespace network_n
{
    class Headers;

    namespace protocol_n
    {
        class HeadersParser_i {
        public:
            virtual ~HeadersParser_i() = default;

            virtual bool operator==(const HeadersParser_i& other) const = 0;

            virtual std::string build(const network_n::Headers& headers) const = 0;
            virtual std::unordered_map<std::string, std::string> parseStartLine(const std::string& startLine) const = 0;
            virtual std::pair<std::string, std::unordered_map<std::string, std::string>> parse(const std::string& stringHeaders) const = 0;
        };
    }
}