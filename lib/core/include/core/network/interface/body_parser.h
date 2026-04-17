#pragma once

#include <core/network/detail/headers.h>
#include <core/network/detail/body.h>
#include <string>


namespace network_n
{
    namespace protocol_n
    {
        template <typename T>
        class BodyParser_i {
        public:
            virtual ~BodyParser_i() = default;

            virtual std::string parse(const Headers& headers, const Body<T>& body) const = 0;
            virtual std::string parse(const Headers& headers, const std::string& stringBody) const = 0;
        };
    }
}