#pragma once

#include <core/network/interface/body_parser.hpp>
#include <string>


class BodyParserMock : public network_n::version_n::BodyParser_i {
public:
    ~BodyParserMock() {}

    static std::shared_ptr<BodyParserMock> get(const std::string& returned) {
        return std::shared_ptr<BodyParserMock>(new BodyParserMock(returned));
    }

    bool operator==(const network_n::version_n::BodyParser_i& other) const override { return dynamic_cast<const BodyParserMock*>(&other) != nullptr; }

    std::vector<std::string> build(const network_n::Headers& headers, const network_n::Body& body) const override { return { m_returned }; }
    std::string parse(const network_n::Headers& headers, std::string_view stringBody) const override { return m_returned; }

private:
    BodyParserMock(const std::string& returned) : m_returned(returned) {}

    std::string m_returned;
};