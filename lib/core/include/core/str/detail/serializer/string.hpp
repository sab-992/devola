#pragma once

#include <core/str/interface/serializer.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>


namespace serializer_n
{
    class String : public Serializer_i<std::string> {
    public:
        String() {}

        friend std::unique_ptr<String> std::make_unique<String>();

        std::string deserialize(std::string_view content) const override;
        std::string serialize(const std::string& object) const override;
    };
}