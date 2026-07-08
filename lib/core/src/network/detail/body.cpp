#include <core/network/detail/body.hpp>


network_n::Body::Body(std::shared_ptr<protocol_n::BodyParser_i> parser) {
    setParser(parser);
}

network_n::Body& network_n::Body::operator=(Body other) {
    swap(*this, other);
    return *this;
}

bool network_n::Body::operator==(const Body& other) const {
    return pointersEqual(m_parser, other.m_parser) and
            m_stringBody == other.m_stringBody;
}

std::vector<std::string> network_n::Body::build(const Headers& headers) const {
    return m_parser->build(headers, *this);
}

void network_n::Body::parse(const Headers& headers, std::string_view stringBody) {
    if (not stringBody.empty())
        m_stringBody = m_parser->parse(headers, stringBody);
}

void network_n::Body::setParser(std::shared_ptr<protocol_n::BodyParser_i> parser) {
    if (parser == nullptr)
        throw InvalidArgument("No parser given", "Body parser");

    m_parser = parser;
}

std::string network_n::Body::toString() const {
    return m_stringBody;
}