#include <core/http/detail/memento/response.hpp>


http_n::memento_n::Response::Response(http_n::Response toSave) {
    using namespace network_n;

    m_body = std::make_unique<Body>(*toSave.m_body);
    m_headers = std::make_unique<Headers>(*toSave.m_headers);
    m_version = toSave.m_version;
    m_status = toSave.m_status;
}

http_n::memento_n::Response::Response(const http_n::memento_n::Response& other) {
    using namespace network_n;

    m_body = std::make_unique<Body>(*other.m_body);
    m_headers = std::make_unique<Headers>(*other.m_headers);
    m_version = other.m_version;
    m_status = other.m_status;
}

bool http_n::memento_n::Response::operator==(const http_n::memento_n::Response& other) const {
    return  pointersEqual(m_body, other.m_body)       and
            pointersEqual(m_headers, other.m_headers) and
            m_version == other.m_version            and
            m_status   == other.m_status;
}