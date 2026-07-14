#pragma once

#include <core/network/detail/headers.hpp>
#include <core/network/interface/body_parser.hpp>
#include <core/str/interface/serializer.hpp>
#include <core/str/detail/serializer/factory.hpp>
#include <core/utility/compare.hpp>
#include <core/utility/string_convertible.hpp>
#include <memory>
#include <string>


namespace network_n
{
    namespace version_n { class BodyParser_i; }

    class Body : public StringConvertible {
        using BodyParser_i = network_n::version_n::BodyParser_i;

    public:
        Body(std::shared_ptr<BodyParser_i> parser);

        Body(const Body& other) = default;
        Body(Body&& other) = default;

        ~Body() = default;

        Body& operator=(Body other);
        Body& operator=(Body&& other) = default;

        bool operator==(const Body& other) const;

        std::vector<std::string> build(const Headers& headers) const;

        template <typename T>
        T convert() const { return serializer<T>()->deserialize(m_stringBody); }

        void parse(const Headers& headers, std::string_view stringBody);

        template <typename T>
        void set(const T& body) { m_stringBody = serializer<T>()->serialize(body); }

        void setParser(std::shared_ptr<BodyParser_i> parser);

        friend void swap(Body& lhs, Body& rhs) {
            std::swap(lhs.m_parser, rhs.m_parser);
            std::swap(lhs.m_stringBody, rhs.m_stringBody);
        }

        std::string toString() const override;

    private:
        std::shared_ptr<BodyParser_i> m_parser;
        std::string m_stringBody;

        template <typename T>
        std::unique_ptr<serializer_n::Serializer_i<T>> serializer() const { return serializer_n::Factory<T>::create(); }
    };
}