#pragma once

#include <core/str/interface/serializer.h>
#include <fstream>
#include <memory>
#include <core/xml/xml.h>
#include <sstream>
#include <string>


namespace serializer_n
{
    class XML : public Serializer_i<xml_n::Document> {
    public:
        XML() {}

        friend std::unique_ptr<XML> std::make_unique<XML>();

        xml_n::Document deserialize(const std::string& content) const override { return xml_n::Document(content); }
        std::string serialize(const xml_n::Document& object) const override { return object.toString(); }
    };
}