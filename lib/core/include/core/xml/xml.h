#pragma once

#include <core/conversion/string_convertible.h>
#include <pugixml.hpp>
#include <sstream>


namespace xml_n
{
    // Wrapper for class xml_n::Document. Used to support comparison of two xml documents.
    class Document : public pugi::xml_document, public StringConvertible {
    public:
        Document(std::string_view document="");
        ~Document() = default;

        bool operator==(const Document& other);
        bool operator!=(const Document& other);

        std::string toString() const override;
    };
}
