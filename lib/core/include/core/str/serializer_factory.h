#pragma once

#include <core/exception.h>
#include <core/str/detail/serializer/json.h>
#include <core/str/detail/serializer/xml.h>
#include <core/str/detail/serializer/string.h>
#include <core/str/interface/serializer.h>
#include <core/xml/xml.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>


namespace serializer_n
{
    template<typename T>
    class Factory {
    public:
        static inline std::unique_ptr<Serializer_i<T>> create() { throw Exception("Not Implemented"); }
    };

    template <>
    inline std::unique_ptr<Serializer_i<nlohmann::json>> Factory<nlohmann::json>::create() { return std::make_unique<JSON>(); }

    template <>
    inline std::unique_ptr<Serializer_i<xml_n::Document>> Factory<xml_n::Document>::create() { return std::make_unique<XML>(); }

    template <>
    inline std::unique_ptr<Serializer_i<std::string>> Factory<std::string>::create() { return std::make_unique<String>(); }
}