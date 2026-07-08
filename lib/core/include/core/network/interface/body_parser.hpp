#pragma once

#include <core/network/detail/headers.hpp>
#include <core/network/detail/body.hpp>
#include <string>


namespace network_n
{
    class Body;

    namespace protocol_n
    {
        class BodyParser_i {
        public:
            virtual ~BodyParser_i() = default;

            virtual bool operator==(const BodyParser_i& other) const = 0;

            virtual std::vector<std::string> build(const Headers& headers, const Body& body) const = 0;
            virtual std::string parse(const Headers& headers, std::string_view stringBody) const = 0;
        };
    }
}