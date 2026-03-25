#pragma once

#include <core/str/detail/serializer/json.h>
#include <core/str/detail/serializer/xml.h>
#include <core/str/detail/serializer/string.h>
#include <core/str/interface/serializer.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <string>


namespace serializer_n
{
    template<typename T>
    class Factory {
    public:
        static inline std::unique_ptr<Serializer_i<T>> create() { throw std::exception("Not implemented"); }
    };

    template <>
    inline std::unique_ptr<Serializer_i<nlohmann::json>> Factory<nlohmann::json>::create() { return std::make_unique<JSON>(); }

    template <>
    inline std::unique_ptr<Serializer_i<pugi::xml_document>> Factory<pugi::xml_document>::create() { return std::make_unique<XML>(); }

    template <>
    inline std::unique_ptr<Serializer_i<std::string>> Factory<std::string>::create() { return std::make_unique<String>(); }
}