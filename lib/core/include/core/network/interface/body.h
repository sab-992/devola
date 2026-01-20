#pragma once

#include <concepts>
#include <core/network/network.h>


namespace network_n
{
    template<typename T>
    class Body_i : virtual public StringFormattable_i {
    public:
        virtual ~Body_i() = default;

        virtual T get() const = 0;
    };
}