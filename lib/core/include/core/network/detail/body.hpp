#pragma once

#include <core/utility/string_convertible.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/interface/body_parser.hpp>
#include <core/str/interface/serializer.hpp>
#include <core/str/detail/serializer/factory.hpp>
#include <core/utility/compare.hpp>
#include <memory>
#include <string>


namespace network_n
{
    namespace protocol_n
    {
        template<typename T>
        class BodyParser_i;
    }

    template<typename T>
    class Body : public StringConvertible {
    public:
        Body(std::shared_ptr<protocol_n::BodyParser_i<T>> parser) { setParser(parser); }

        Body(const Body<T>& other) = default;
        Body(Body<T>&& other) = default;

        ~Body() = default;

        Body<T>& operator=(Body<T> other) { swap(*this, other); return *this; }
        Body<T>& operator=(Body<T>&& other) = default;

        bool operator==(const Body<T>& other) const {
            return pointersEqual(m_parser, other.m_parser) and
                   m_stringBody == other.m_stringBody;
        }

        std::vector<std::string> build(const Headers& headers) const { return m_parser->build(headers, *this); }

        T convert() const { return serializer()->deserialize(m_stringBody); }

        void parse(const Headers& headers, std::string_view stringBody) {
            if (not stringBody.empty())
                m_stringBody = m_parser->parse(headers, stringBody);
        }

        void set(const T& body) { m_stringBody = serializer()->serialize(body); }

        void setParser(std::shared_ptr<protocol_n::BodyParser_i<T>> parser) {
            if (parser == nullptr)
                throw InvalidArgument("No parser given", "Body parser");

            m_parser = parser;
        }

        friend void swap(Body<T>& lhs, Body<T>& rhs) {
            std::swap(lhs.m_parser, rhs.m_parser);
            std::swap(lhs.m_stringBody, rhs.m_stringBody);
        }

        std::string toString() const override { return m_stringBody; }

    private:
        std::shared_ptr<protocol_n::BodyParser_i<T>> m_parser;
        std::string m_stringBody;

        std::unique_ptr<serializer_n::Serializer_i<T>> serializer() const {
            return serializer_n::Factory<T>::create();
        }
    };
}