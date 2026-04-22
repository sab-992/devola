#pragma once

#include <core/conversion/string_convertible.h>
#include <core/network/detail/headers.h>
#include <core/network/interface/body_parser.h>
#include <core/str/interface/serializer.h>
#include <core/str/serializer_factory.h>
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
        Body(std::unique_ptr<protocol_n::BodyParser_i<T>> parser) { setParser(std::move(parser)); }

        Body(const Body<T>& other) { m_stringBody = other.m_stringBody; }
        Body(Body<T>&& other) { m_stringBody = std::move(other.m_stringBody); }

        ~Body() {}

        Body<T>& operator=(Body<T> other) {
            swap(*this, other);
            return *this;
        }

        bool operator==(const Body<T>& other) const {
            // IF UPDATED, ALSO UPDATE THE COMMENT IN http_n::Headers
            // The parsers are not dereferenced before the comparison because they are singletons
            // (will always be the same pointee if coming from the same parser class).
            // Moreover, they are stateless, meaning that are the same from creation to destruction.
            return m_parser     == m_parser and
                   m_stringBody == other.m_stringBody;
        }

        std::vector<std::string> build(const Headers& headers) const { return m_parser->build(headers, *this); }

        T convert() const { return serializer()->deserialize(m_stringBody); }

        void parse(const Headers& headers, const std::string& stringBody) {
            if (not stringBody.empty())
                m_stringBody = m_parser->parse(headers, stringBody);
        }

        void set(const T& body) { m_stringBody = serializer()->serialize(body); }

        void setParser(std::unique_ptr<protocol_n::BodyParser_i<T>> parser) {
            if (parser == nullptr)
                throw InvalidArgument("No parser given", "Body parser");

            m_parser = std::move(parser);
        }

        friend void swap(Body<T>& lhs, Body<T>& rhs) { std::swap(lhs.m_stringBody, rhs.m_stringBody); }

        std::string toString() const override { return m_stringBody; }

    private:
        std::unique_ptr<protocol_n::BodyParser_i<T>> m_parser = nullptr;
        std::string m_stringBody;

        std::unique_ptr<serializer_n::Serializer_i<T>> serializer() const {
            return serializer_n::Factory<T>::create();
        }
    };
}