#include <core/network/detail/headers.h>


network_n::Headers::Headers(const std::string& stringHeaders) {
    if (not stringHeaders.empty())
        parse(stringHeaders);
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

network_n::Headers& network_n::Headers::operator=(Headers other) {
    swap(*this, other);
    return *this;
}

std::string network_n::Headers::get(std::string name) const {
    return m_headersMap.contains(name) ? m_headersMap.at(name) : "";
}

std::string network_n::Headers::startLine() const {
    return m_startLine;
}

void network_n::Headers::parse(const std::string& stringHeaders) {
    std::istringstream input(trim(stringHeaders));
    std::string line;

    std::getline(input, line);
    setStartLine(line);

    for (; std::getline(input, line);) {
        line = trim(line);
        size_t separator = line.find(":");

        if (separator == std::string::npos)
            throw std::invalid_argument(std::format("Header: {} is ill-formed", line));

        std::string name = line.substr(0, separator);
        std::string value = trim(line.substr(separator + 1));

        setHeader(name, value);
    }
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