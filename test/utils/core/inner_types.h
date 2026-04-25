#pragma once

#include <core/exception.h>
#include <core/xml/xml.h>
#include <nlohmann/json.hpp>
#include <string>
#include <utils/core/body_parser_mock.h>


template <template <typename> class Object>
using networkInnerTypes_t = ::testing::Types<Object<std::string>, Object<nlohmann::json>, Object<xml_n::Document>>;

template <typename T, template <typename...> class Object>
class InnerTypes {
public:
    inline static auto getTestObject(const std::string& stringContent) {
        if constexpr (std::is_base_of_v<typename Object<nlohmann::json>::type, T>)
            return nlohmann::json::parse(stringContent);
        if constexpr (std::is_base_of_v<typename Object<std::string>::type, T>)
            return stringContent;
        if constexpr (std::is_base_of_v<typename Object<xml_n::Document>::type, T>)
            return xml_n::Document(stringContent);
        throw Exception("Unhandled single templated type object");
    }

    inline static std::string getTestStringObject(bool alt) {
        if constexpr (std::is_base_of_v<typename Object<nlohmann::json>::type, T>)
            return getTestObject(createJSONString(alt)).dump();
        if constexpr (std::is_base_of_v<typename Object<std::string>::type, T>)
            return getTestObject(createTextString(alt));
        if constexpr (std::is_base_of_v<typename Object<xml_n::Document>::type, T>)
            return getTestObject(createXMLString(alt)).toString();
        throw Exception("Unhandled single templated type object");
    }

    inline static auto getBodyParserMock(bool alt) {
        const std::string mockedResult = getTestStringObject(alt);
        if constexpr (std::is_base_of_v<typename Object<nlohmann::json>::type, T>)
            return std::make_unique<BodyParserMock<nlohmann::json>>(mockedResult);
        if constexpr (std::is_base_of_v<typename Object<std::string>::type, T>)
            return std::make_unique<BodyParserMock<std::string>>(mockedResult);
        if constexpr (std::is_base_of_v<typename Object<xml_n::Document>::type, T>)
            return std::make_unique<BodyParserMock<xml_n::Document>>(mockedResult);
        throw Exception("Unhandled single templated type object");
    }

private:
    inline static std::string createJSONString(bool alt) {
        return alt ? R"({ "alternateTest": "works!", "json": true })" : R"({ "test": "works!", "json": true })";
    }

    inline static std::string createTextString(bool alt) {
        return alt ? "Hello alternate test!" : "Hello test!";
    }

    inline static std::string createXMLString(bool alt) {
        return alt ? "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello alternate xml test!</item>\n</root>\n" : 
                     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello xml test!</item>\n</root>\n";
    }
};