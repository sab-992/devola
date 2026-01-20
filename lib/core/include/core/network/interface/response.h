#pragma once

#include <string>

 
namespace network_n
{
    template <typename T>
    class Response_i : virtual public network_n::Message_i<T> {
    public:
        virtual ~Response_i() = default;

        virtual network_n::Status status() const = 0;
    };
}