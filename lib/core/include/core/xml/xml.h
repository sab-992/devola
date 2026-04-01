#pragma once

#include <pugixml.hpp>
#include <core/conversion/string_convertible.h>
#include <sstream>

namespace xml_n
{
    // Wrapper for class xml_n::Document. Used to support comparison of two xml documents.
    class Document : public pugi::xml_document, public StringConvertible {
    public:
        Document(const std::string& document="");

        bool operator==(const Document& other);
        inline bool operator!=(const Document& other);

        std::string toString() const override;
    };
}
