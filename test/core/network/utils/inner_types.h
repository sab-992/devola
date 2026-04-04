#pragma once

#include <core/xml/xml.h>
#include <nlohmann/json.hpp>
#include <string>

template <template <typename> class Object>
using networkInnerTypes_t = ::testing::Types<Object<std::string>, Object<nlohmann::json>, Object<xml_n::Document>>;