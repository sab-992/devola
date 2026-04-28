#pragma once

#include <core/network/interface/body_parser.h>
#include <string>


template<typename T>
class BodyParserMock : public network_n::protocol_n::BodyParser_i<T> {
public:
    ~BodyParserMock() {}

    static std::unique_ptr<BodyParserMock<T>> get(const std::string& returned) {
        return std::unique_ptr<BodyParserMock<T>>(new BodyParserMock<T>(returned));
    }

    std::vector<std::string> build(const network_n::Headers& headers, const network_n::Body<T>& body) const override { return { m_returned }; }
    std::string parse(const network_n::Headers& headers, const std::string& stringBody) const override { return m_returned; }
private:
    BodyParserMock(const std::string& returned) : m_returned(returned) {}

    std::string m_returned;
};