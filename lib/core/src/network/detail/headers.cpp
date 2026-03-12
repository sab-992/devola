#include <core/network/detail/headers.h>


network_n::Headers::Headers() {}

std::string network_n::Headers::getHeader(std::string name) const {
    return m_headersMap.contains(name) ? m_headersMap.at(name) : "";
}

std::string network_n::Headers::getStartLine() const {
    return m_startLine;
}

void network_n::Headers::setHeader(std::string name, std::string value) {
    m_headersMap[name] = value;
}

void network_n::Headers::setStartLine(std::string startLine) {
    m_startLine = startLine;
}

headersUMap_t network_n::Headers::toMap() const {
    return m_headersMap;
}

std::string network_n::Headers::toString() const {
    std::string headers = m_startLine;

    for (auto& [header, value] : m_headersMap)
        headers += std::format("\r\n{}: {}", header, value);

    return trim(headers);
}