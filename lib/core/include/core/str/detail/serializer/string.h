#pragma once

#include <core/str/interface/serializer.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>


namespace serializer_n
{
    class String : public Serializer_i<std::string> {
    public:
        String() {}

        friend std::unique_ptr<String> std::make_unique<String>();

        std::string deserialize(std::string content) const override { return content; }
        std::string serialize(const std::string& object) const override { return object; }
    };
}