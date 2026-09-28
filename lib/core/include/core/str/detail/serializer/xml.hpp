#pragma once

#include <core/str/interface/serializer.hpp>
#include <core/xml/document.hpp>
#include <memory>
#include <string>


namespace serializer_n
{
    class XML : public Serializer_i<xml_n::Document> {
    public:
        XML() {}

        friend std::unique_ptr<XML> std::make_unique<XML>();

        xml_n::Document deserialize(std::string_view content) const override;
        std::string serialize(const xml_n::Document& object) const override;
    };
}