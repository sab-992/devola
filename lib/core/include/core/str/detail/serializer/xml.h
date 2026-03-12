#pragma once

#include <core/str/interface/serializer.h>
#include <fstream>
#include <memory>
#include <pugixml.hpp>
#include <sstream>
#include <string>


namespace serializer_n
{
    class XML : public Serializer_i<pugi::xml_document> {
    public:
        XML() {}

        friend std::unique_ptr<XML> std::make_unique<XML>();

        pugi::xml_document deserialize(const std::string content) const override { 
            pugi::xml_document doc;
            doc.load_string(content.c_str());
            return doc;
        }

        std::string serialize(const pugi::xml_document& object) const override {
            std::stringstream ss;
            object.save(ss);
            return ss.str();
        }
    };
}