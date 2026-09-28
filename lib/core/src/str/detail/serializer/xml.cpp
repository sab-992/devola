#include <core/str/detail/serializer/xml.hpp>


xml_n::Document serializer_n::XML::deserialize(std::string_view content) const {
    return xml_n::Document(content);
}

std::string serializer_n::XML::serialize(const xml_n::Document& object) const {
    return object.toString();
}