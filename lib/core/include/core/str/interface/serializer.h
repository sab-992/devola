#pragma once

#include <string>


namespace serializer_n
{
    template<typename T>
    class Serializer_i {
    public:
        virtual T deserialize(std::string content) const = 0;
        virtual std::string serialize(const T& object) const = 0;
    };
}