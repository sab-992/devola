#include <core/str/detail/serializer/string.hpp>


std::string serializer_n::String::deserialize(std::string_view content) const {
    return std::string(content);
}

std::string serializer_n::String::serialize(const std::string& object) const {
    return object;
}