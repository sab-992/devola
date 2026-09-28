#pragma once

#include <core/str/interface/serializer.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>


namespace serializer_n
{
    class JSON : public Serializer_i<nlohmann::json> {
    public:
        JSON() {}

        friend std::unique_ptr<JSON> std::make_unique<JSON>();

        nlohmann::json deserialize(std::string_view content) const override;
        std::string serialize(const nlohmann::json& object) const override;
    };
}