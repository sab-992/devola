#pragma once

#include <core/exception.h>
#include <core/xml/xml.h>
#include <nlohmann/json.hpp>
#include <string>
#include <utils/core/body_parser_mock.h>


using networkInnerTypes_t = ::testing::Types<std::string, nlohmann::json, xml_n::Document>;

template <template <typename> class Object>
using networkTemplatedInnerTypes_t = ::testing::Types<Object<std::string>, Object<nlohmann::json>, Object<xml_n::Document>>;

template<typename T>
class InnerTypes {
public:
    inline static auto getTestObject(const std::string& stringContent) { throw Exception("Type not handled"); }
    inline static std::string getTestString(bool alt) { throw Exception("Type not handled"); }
};

template<>
inline auto InnerTypes<nlohmann::json>::getTestObject(const std::string& stringContent) { return nlohmann::json::parse(stringContent); }
template<>
inline auto InnerTypes<std::string>::getTestObject(const std::string& stringContent) { return stringContent; }
template<>
inline auto InnerTypes<xml_n::Document>::getTestObject(const std::string& stringContent) { return xml_n::Document(stringContent); }

template<>
inline std::string InnerTypes<nlohmann::json>::getTestString(bool alt) { return alt ? R"({ "alternateTest": "works!", "json": true })" :
                                                                                      R"({ "test": "works!", "json": true })"; }
template<>
inline std::string InnerTypes<std::string>::getTestString(bool alt) { return alt ? "Hello alternate test!" :
                                                                                   "Hello test!"; }
template<>
inline std::string InnerTypes<xml_n::Document>::getTestString(bool alt) { return alt ? "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello alternate xml test!</item>\n</root>\n" :
                                                                                       "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<item>Hello xml test!</item>\n</root>\n"; }

template <typename T, template <typename...> class Object>
class InnerTemplatedTypes {
public:
    inline static auto getTestObject(const std::string& stringContent) {
        if constexpr (std::is_base_of_v<typename Object<nlohmann::json>::type, T>)
            return InnerTypes<nlohmann::json>::getTestObject(stringContent);
        if constexpr (std::is_base_of_v<typename Object<std::string>::type, T>)
            return stringContent;
        if constexpr (std::is_base_of_v<typename Object<xml_n::Document>::type, T>)
            return InnerTypes<xml_n::Document>::getTestObject(stringContent);

        throw Exception("Unhandled single templated type object");
    }

    // Need to create an object then convert to string to match formatting.
    inline static std::string getTestStringObject(bool alt) {
        if constexpr (std::is_base_of_v<typename Object<nlohmann::json>::type, T>)
            return InnerTypes<nlohmann::json>::getTestObject(InnerTypes<nlohmann::json>::getTestString(alt)).dump();
        if constexpr (std::is_base_of_v<typename Object<std::string>::type, T>)
            return InnerTypes<std::string>::getTestString(alt);
        if constexpr (std::is_base_of_v<typename Object<xml_n::Document>::type, T>)
            return InnerTypes<xml_n::Document>::getTestObject(InnerTypes<xml_n::Document>::getTestString(alt)).toString();

        throw Exception("Unhandled single templated type object");
    }

    inline static auto getBodyParserMock(bool alt) {
        const std::string mockedResult = getTestStringObject(alt);
        if constexpr (std::is_base_of_v<typename Object<nlohmann::json>::type, T>)
            return BodyParserMock<nlohmann::json>::get(mockedResult);
        if constexpr (std::is_base_of_v<typename Object<std::string>::type, T>)
            return BodyParserMock<std::string>::get(mockedResult);
        if constexpr (std::is_base_of_v<typename Object<xml_n::Document>::type, T>)
            return BodyParserMock<xml_n::Document>::get(mockedResult);

        throw Exception("Unhandled single templated type object");
    }
};