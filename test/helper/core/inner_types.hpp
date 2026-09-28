#pragma once

#include <core/exception.hpp>
#include <core/xml/document.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <helper/core/body_parser_mock.hpp>


using innerTypes_t = ::testing::Types<std::string, nlohmann::json, xml_n::Document>;

template<typename T>
class InnerTypes {
public:
    inline static auto getTestObject(const std::string& stringContent) { throw Exception("Type not handled"); }
    inline static std::string getTestString(bool alt) { throw Exception("Type not handled"); }
    inline static std::string getTestStringFromObject(const T& object) { throw Exception("Type not handled"); }
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

template<>
inline std::string InnerTypes<nlohmann::json>::getTestStringFromObject(const nlohmann::json& object) { return object.dump(); }
template<>
inline std::string InnerTypes<std::string>::getTestStringFromObject(const std::string& object) { return object; }
template<>
inline std::string InnerTypes<xml_n::Document>::getTestStringFromObject(const xml_n::Document& object) { return object.toString(); }