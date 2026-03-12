#pragma once

#include <core/conversion/string_convertible.h>
#include <core/str/interface/serializer.h>
#include <core/str/serializer_factory.h>
#include <memory>
#include <string>


namespace network_n
{
    template<typename T>
    class Body : public StringConvertible {
    public:
        Body() {}
        Body(std::string stringBody) : m_stringBody(stringBody) {}

        T convert() const { return m_serializer->deserialize(m_stringBody); }

        void set(const T& body) { m_stringBody = m_serializer->serialize(body); }

        std::string toString() const override { return m_stringBody; }

    private:
        std::unique_ptr<serializer_n::Serializer_i<T>> m_serializer = serializer_n::Factory<T>::get();
        std::string m_stringBody;
    };
}