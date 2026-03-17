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
        Body(const Body<T>& other) { m_stringBody = other.m_stringBody; }
        Body(Body<T>&& other) { m_stringBody = std::move(other.m_stringBody); }

        ~Body() {}

        Body<T>& operator=(Body<T> other) {
            swap(*this, other);
            return *this;
        }

        friend void swap(Body<T>& lhs, Body<T>& rhs) { std::swap(lhs.m_stringBody, rhs.m_stringBody); }

        T convert() const { return serializer()->deserialize(m_stringBody); }
        void set(const T& body) { m_stringBody = serializer()->serialize(body); }

        std::string toString() const override { return m_stringBody; }

    private:
        std::string m_stringBody;

        std::unique_ptr<serializer_n::Serializer_i<T>> serializer() const {
            return serializer_n::Factory<T>::get();
        }
    };
}