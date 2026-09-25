#include <core/str/detail/serializer/json.hpp>


nlohmann::json serializer_n::JSON::deserialize(std::string_view content) const {
    return nlohmann::json::parse(content);
}

std::string serializer_n::JSON::serialize(const nlohmann::json& object) const {
    return object.dump();
}