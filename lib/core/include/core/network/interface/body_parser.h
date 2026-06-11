#pragma once

#include <core/network/detail/headers.h>
#include <core/network/detail/body.h>
#include <string>


namespace network_n
{
    template<typename T>
    class Body;

    namespace protocol_n
    {
        template <typename T>
        class BodyParser_i {
        public:
            virtual ~BodyParser_i() = default;

            virtual bool operator==(const BodyParser_i<T>& other) const = 0;

            virtual std::vector<std::string> build(const Headers& headers, const Body<T>& body) const = 0;
            virtual std::string parse(const Headers& headers, std::string_view stringBody) const = 0;
        };
    }
}