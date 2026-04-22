#include <core/network/detail/headers.h>


network_n::Headers::Headers(std::unique_ptr<protocol_n::HeadersParser_i> parser) {
    setParser(std::move(parser));
}

network_n::Headers::Headers(const Headers& other)  {
    m_headersMap = other.m_headersMap;
    m_startLine = other.m_startLine;
}

network_n::Headers::Headers(Headers&& other) {
    m_headersMap = std::move(other.m_headersMap);
    m_startLine = std::move(other.m_startLine);
}

network_n::Headers::~Headers() {}

network_n::Headers& network_n::Headers::operator=(Headers other)  {
    swap(*this, other);
    return *this;
}

bool network_n::Headers::operator==(const Headers& other) const {
    // IF UPDATED, ALSO UPDATE THE COMMENT IN http_n::Body
    // The parsers are not dereferenced before the comparison because they are singletons
    // (will always be the same pointee if coming from the same parser class).
    // Moreover, they are stateless, meaning that are the same from creation to destruction.
    return m_headersMap == other.m_headersMap and
           m_parser     == other.m_parser     and
           m_startLine  == m_startLine;
}

std::string network_n::Headers::build() const {
    return toString();
}

std::string network_n::Headers::get(const std::string& name) const {
    return m_headersMap.contains(name) ? m_headersMap.at(name) : "";
}

void network_n::Headers::parse(const std::string& stringHeaders) {
    if (stringHeaders.empty())
        return;

    const auto& [startline, headersUMap] = m_parser->parse(stringHeaders);

    m_headersMap = headersUMap;
    m_startLine = startline;
}

std::string network_n::Headers::startLine() const {
    return m_startLine;
}

void network_n::Headers::setHeader(const std::string& name, const std::string& value) {
    m_headersMap[name] = value;
}

void network_n::Headers::setParser(std::unique_ptr<protocol_n::HeadersParser_i> parser) {
    if (parser == nullptr)
        throw InvalidArgument("No parser given", "Headers parser");

    m_parser = std::move(parser);
}

void network_n::Headers::setStartLine(const std::string& startLine) {
    m_startLine = trim(startLine);
}

headersUMap_t network_n::Headers::toMap() const {
    return m_headersMap;
}

std::string network_n::Headers::toString() const {
    return m_parser->build(*this);
}