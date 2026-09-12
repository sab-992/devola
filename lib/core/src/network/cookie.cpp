#include <core/network/cookie.hpp>


network_n::Cookie::Cookie() {}

network_n::Cookie::Cookie(std::string_view cookie) {
    parse(cookie);
}

network_n::Cookie& network_n::Cookie::build() & {
    return *this;
}

network_n::Cookie network_n::Cookie::build() && {
    return *this;
}

bool network_n::Cookie::isBrowserOnly() const {
    return m_isBrowserOnly;
}

std::chrono::seconds network_n::Cookie::maxAge() const {
    return m_maxAge;
}

const std::string& network_n::Cookie::name() const {
    return m_name;
}

const std::string& network_n::Cookie::path() const {
    return m_path;
}

network_n::Cookie& network_n::Cookie::setMaxAge(std::chrono::seconds ttl) {
    m_maxAge = ttl;
    return *this;
}

network_n::Cookie& network_n::Cookie::setName(std::string_view name) {
    m_name = name;
    return *this;
}

network_n::Cookie& network_n::Cookie::setPath(std::string_view path) {
    m_path = path;
    return *this;
}

network_n::Cookie& network_n::Cookie::setRestrictionToBrowser(bool isBrowserOnly) {
    m_isBrowserOnly = isBrowserOnly;
    return *this;
}

network_n::Cookie& network_n::Cookie::setValue(std::string_view value) {
    m_value = value;
    return *this;
}

void network_n::Cookie::parse(std::string_view cookie) {
    const std::vector<std::string>& parts = split(cookie, "=");
    if (parts.size() != 2)
        throw InvalidArgument("Cookie structure is invalid");

    m_name   = trim(parts[0]);
    m_value = trim(parts[1]);
}

std::string network_n::Cookie::buildForTransmission() const {
    return std::format("{}={};{} Secure; Max-Age={}; SameSite=Strict; Path={}", m_name,
                                                                                m_value,
                                                                                m_isBrowserOnly ? " HttpOnly;" : "",
                                                                                m_maxAge.count(),
                                                                                m_path);
};

std::string network_n::Cookie::toString() const {
    return std::format("{}={}", m_name, m_value);
}

const std::string& network_n::Cookie::value() const {
    return m_value;
}