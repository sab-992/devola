#pragma once

#include <core/exception.h>
#include <core/xml/xml.h>
#include <nlohmann/json.hpp>
#include <string>


template <template <typename> class Object>
using networkInnerTypes_t = ::testing::Types<Object<std::string>, Object<nlohmann::json>, Object<xml_n::Document>>;

template <typename T, template <typename...> class Object>
class InnerTypes {
public:
    inline static auto GetTestBody(bool alt=false) {
        if constexpr (std::is_base_of_v<typename Object<nlohmann::json>::type, T>)
            return createJSON(alt);
        if constexpr (std::is_base_of_v<typename Object<std::string>::type, T>)
            return createString(alt);
        if constexpr (std::is_base_of_v<typename Object<xml_n::Document>::type, T>)
            return createXML(alt);
        throw Exception("Unhandled single templated type object");
    }

private:
    static nlohmann::json createJSON(bool alt) {
        const std::string content = alt ? R"({ "alternateTest": "works!", "json": true })" : R"({ "test": "works!", "json": true })";
        return nlohmann::json::parse(content);
    }

    static std::string createString(bool alt) {
        return std::string(alt ? "Hello alternate test!" : "Hello test!");
    }

    static xml_n::Document createXML(bool alt) {
        const std::string content = alt ? "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello alternate xml test!</item>\n</root>\n" : 
                                          "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello xml test!</item>\n</root>\n";
        return xml_n::Document(content);
    }
};