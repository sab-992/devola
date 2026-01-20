#pragma once

#include <concepts>
#include <core/network/network.h>
#include <core/utils/converter.h>
#include <string>


namespace network_n
{
    template <typename T>
    class Message_i : virtual public StringFormattable_i {
    public:
        virtual ~Message_i() = default;

        virtual T body() const = 0;
        virtual std::string getHeader(std::string Header) const = 0;
        virtual std::string headers() const = 0;
        virtual HeadersUMap_t headersMap() const = 0;
    };
}