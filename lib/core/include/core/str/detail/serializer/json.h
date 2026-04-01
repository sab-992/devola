#pragma once

#include <core/str/interface/serializer.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>


namespace serializer_n
{
    class JSON : public Serializer_i<nlohmann::json> {
    public:
        JSON() {}

        friend std::unique_ptr<JSON> std::make_unique<JSON>();

        nlohmann::json deserialize(const std::string& content) const override { return nlohmann::json::parse(content); }
        std::string serialize(const nlohmann::json& object) const override { return object.dump(); }
    };
}