#pragma once

#include <string>


namespace serializer_n
{
    // Any class implementing this interface NEED to be stateless.
    template<typename T>
    class Serializer_i {
    public:
        virtual ~Serializer_i() = default;

        virtual T deserialize(const std::string& content) const = 0;
        virtual std::string serialize(const T& object) const = 0;
    };
}