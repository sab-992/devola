#pragma once

#include <core/utility/string_convertible.hpp>
#include <pugixml.hpp>
#include <sstream>


namespace xml_n
{
    // Wrapper for class xml_n::Document. Used to support comparison of two xml documents.
    class Document : public pugi::xml_document, public StringConvertible {
    public:
        Document(std::string_view document="");
        ~Document() = default;

        Document(const Document&) = delete;
        Document& operator=(const Document&) = delete;

        Document(Document&&) noexcept = default;
        Document& operator=(Document&&) noexcept = default;

        bool operator==(const Document& other);
        bool operator!=(const Document& other);

        std::string toString() const override;
    };
}
