#pragma once

#include <core/network/network.h>
#include <core/str/string_formattable.h>
#include <string>
#include <unordered_map>


namespace network_n
{
    class Headers_i : virtual public StringFormattable_i {
    public:
        virtual ~Headers_i() = default;

        virtual std::string apiEndpoint() const = 0;
        virtual std::string get() const = 0;
        virtual std::string getHeader(std::string Header) const = 0;
        virtual HeadersUMap_t map() const = 0;
        virtual network_n::Status status() const = 0;
    protected:
        virtual std::string extractMessageInformation(std::string RawHeader) = 0;
    };
}