#pragma once

#include <core/exception.hpp>
#include <core/str/detail/serializer/json.hpp>
#include <core/str/detail/serializer/xml.hpp>
#include <core/str/detail/serializer/string.hpp>
#include <core/str/interface/serializer.hpp>
#include <core/xml/document.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>


namespace serializer_n
{
    template<typename T>
    class Factory {
    public:
        inline static std::unique_ptr<Serializer_i<T>> create() { throw Exception("Not Implemented"); }
    };

    template <>
    inline std::unique_ptr<Serializer_i<nlohmann::json>> Factory<nlohmann::json>::create() { return std::make_unique<JSON>(); }

    template <>
    inline std::unique_ptr<Serializer_i<xml_n::Document>> Factory<xml_n::Document>::create() { return std::make_unique<XML>(); }

    template <>
    inline std::unique_ptr<Serializer_i<std::string>> Factory<std::string>::create() { return std::make_unique<String>(); }
}