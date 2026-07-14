#include <core/http/detail/memento/request.hpp>


http_n::memento_n::Request::Request(const http_n::Request& toSave) {
    using namespace network_n;

    m_APIEndpoint = toSave.m_APIEndpoint;
    m_body = std::make_unique<Body>(*toSave.m_body);
    m_headers = std::make_unique<Headers>(*toSave.m_headers);
    m_method = toSave.m_method;
    m_port = toSave.m_port;
    m_version = toSave.m_version;
    m_URL = toSave.m_URL;
}

http_n::memento_n::Request::Request(const Request& other) {
    using namespace network_n;

    m_APIEndpoint = other.m_APIEndpoint;
    m_body = std::make_unique<Body>(*other.m_body);
    m_headers = std::make_unique<Headers>(*other.m_headers);
    m_port = other.m_port;
    m_version = other.m_version;
    m_URL = other.m_URL;
}

bool http_n::memento_n::Request::operator==(const http_n::memento_n::Request& other) const {
    return  m_APIEndpoint ==  other.m_APIEndpoint     and
            pointersEqual(m_body, other.m_body)       and
            pointersEqual(m_headers, other.m_headers) and
            m_method      ==  other.m_method          and
            m_port        ==  other.m_port            and
            m_version    ==  other.m_version        and
            m_URL         ==  other.m_URL;
}