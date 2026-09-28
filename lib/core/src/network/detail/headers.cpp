#include <core/network/detail/headers.hpp>


network_n::Headers::Headers(std::shared_ptr<HeadersParser_i> parser) {
    setParser(parser);
}

network_n::Headers& network_n::Headers::operator=(Headers other)  {
    swap(*this, other);
    return *this;
}

bool network_n::Headers::operator==(const Headers& other) const {
    return m_headersMap == other.m_headersMap      and
           pointersEqual(m_parser, other.m_parser) and
           m_startLine  == m_startLine;
}

std::string network_n::Headers::build() const {
    return m_parser->build(*this);
}

const network_n::Cookie& network_n::Headers::cookie(const std::string& name) const {
    if (m_setCookies.contains(name))
        return m_setCookies.at(name);
    else if (m_cookies.contains(name))
        return m_cookies.at(name);

    throw Exception(std::format("Cookie \"{}\" not found", name));
}

std::string network_n::Headers::get(const std::string& name) const {
    return m_headersMap.contains(name) ? m_headersMap.at(name) : "";
}

void network_n::Headers::parse(std::string_view stringHeaders) {
    if (stringHeaders.empty())
        return;

    const auto& [startLine, headersUMap, cookies] = m_parser->parse(stringHeaders);

    m_headersMap = headersUMap;
    m_startLine = startLine;
    m_cookies = cookies;
}

void network_n::Headers::setCookie(const std::string& name, Cookie value) {
    m_setCookies[name] = value;
}

void network_n::Headers::setHeader(const std::string& name, std::string_view value) {
    m_headersMap[name] = value;
}

void network_n::Headers::setParser(std::shared_ptr<HeadersParser_i> parser) {
    if (parser == nullptr)
        throw InvalidArgument("No parser given", "Headers parser");

    m_parser = parser;
}

void network_n::Headers::setStartLine(std::string_view startLine) {
    m_startLine = trim(startLine);
}

std::string network_n::Headers::startLine() const {
    return m_startLine;
}

cookies_t network_n::Headers::addedCookiesToMap() const {
    return m_setCookies;
}

cookies_t network_n::Headers::cookiesToMap() const {
    return m_cookies;
}

headers_t network_n::Headers::headersToMap() const {
    return m_headersMap;
}

std::string network_n::Headers::toString() const {
    return build();
}