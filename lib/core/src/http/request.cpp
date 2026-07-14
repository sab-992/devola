#include <core/http/request.hpp>


http_n::Request::Request()
: Message<Request>(http_n::DEFAULT_PROTOCOL), m_port(this->version()->defaultPort()) {}

http_n::Request::Request(Headers headers, Body body)
: Message<http_n::Request>(http_n::DEFAULT_PROTOCOL), m_port(this->version()->defaultPort()) {
    Message<http_n::Request>::set(std::move(headers), std::move(body));

    const startLineInformation_t& requestInfo = this->version()->headersParser()->parseStartLine(this->m_headers->startLine());
    initFromStartLineInformation(requestInfo);
}

http_n::Request::Request(const Request& other) : Message<Request>(other) {
    m_lastBuild = other.m_lastBuild ? std::make_unique<memento_n::Request>(*other.m_lastBuild) : nullptr;
    m_APIEndpoint = other.m_APIEndpoint;
    m_method = other.m_method;
    m_port = other.m_port;
    m_URL = other.m_URL;
}

http_n::Request& http_n::Request::operator=(Request other) {
    swap(*this, other);
    return *this;
}

std::string http_n::Request::APIEndpoint() const {
    return m_APIEndpoint;
}

void http_n::Request::finalize() {
    validateMembers();
    this->setStartLine(std::format("{} {} {}", m_method, m_APIEndpoint, this->version()->name()));
    this->m_headers->setHeader("Host", std::format("{}:{}", m_URL, m_port));
}

bool http_n::Request::hasChangedSinceLastBuild() const {
    // If m_lastBuild is nullptr, build() was never called and since this
    // is not a static function, we are guaranteed that an object
    // has been created, therefore the request has changed.
    if (not m_lastBuild)
        return true;

    return *m_lastBuild != memento_n::Request(*this);
}

void http_n::Request::initFromStartLineInformation(const startLineInformation_t& information) {
    setMethod(information[0]);
    setAPIEndpoint(information[1]);

    const std::string& host = this->header("Host");
    if (host.empty()) return;
    const auto& [url, port] = parseHostURL(host);

    setURL(url);
    setPort(port);
}

std::string http_n::Request::method() const {
    return m_method;
}

void http_n::Request::parse(std::string_view stringRequest) {
    const startLineInformation_t& requestInfo = this->processMessage(stringRequest);
    initFromStartLineInformation(requestInfo);
}

std::pair<std::string_view, uint16_t> http_n::Request::parseHostURL(std::string_view host) {
    size_t separatorIndex = host.find(':');

    if (trim(host).empty() or separatorIndex == std::string::npos)
        return { "", m_port };

    return { host.substr(0, separatorIndex), static_cast<uint16_t>(std::stoi(std::string(host.substr(separatorIndex + 1)))) };
}

uint16_t http_n::Request::port() const {
    return m_port;
}

http_n::Request& http_n::Request::set(std::string_view stringRequest) {
    parse(stringRequest);
    return *this;
}

http_n::Request& http_n::Request::setAPIEndpoint(std::string_view endpoint) {
    m_APIEndpoint = endpoint;
    return *this;
}

http_n::Request& http_n::Request::setMethod(std::string_view method) {
    m_method = method;
    return *this;
}

http_n::Request& http_n::Request::setPort(uint16_t port) {
    m_port = port;
    return *this;
}

http_n::Request& http_n::Request::setURL(std::string_view url) {
    m_URL = url;
    return *this;
}

std::string http_n::Request::url() const {
    return m_URL;
}

void http_n::Request::updateLastBuild() {
    m_lastBuild = std::make_unique<memento_n::Request>(*this);
}

void http_n::Request::validateMembers() const {
    if (m_APIEndpoint.empty()) throw InvalidArgument("Cannot be empty", "API endpoint");
    if (m_method.empty()) throw InvalidArgument("Cannot be empty", "HTTP method");
    if (m_port == 0) throw InvalidArgument("Cannot be '0'", "Host port");
    if (m_URL.empty()) throw InvalidArgument("Cannot be empty", "Host URL");
}