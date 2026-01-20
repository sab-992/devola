#pragma once

#include <string>

 
namespace network_n
{
    template <typename T>
    class Request_i : virtual public network_n::Message_i<T> {
    public:
        virtual ~Request_i() = default;

        virtual std::string apiEndpoint() const = 0;
    };
}