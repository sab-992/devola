#include <core/http/response.hpp>


http_n::Response::Response() : Message<Response>(http_n::DEFAULT_PROTOCOL) {}

http_n::Response::Response(Headers headers, Body body) : Message<Response>(http_n::DEFAULT_PROTOCOL) {
    Message<Response>::set(std::move(headers), std::move(body));

    const startLineInformation_t& responseInfo = this->version()->headersParser()->parseStartLine(this->m_headers->startLine());
    initFromStartLineInformation(responseInfo);
}

http_n::Response::Response(const Response& other) : Message<Response>(other) {
    m_lastBuild = other.m_lastBuild ? std::make_unique<memento_n::Response>(*other.m_lastBuild) : nullptr;
    m_status = other.m_status;
}

http_n::Response& http_n::Response::operator=(Response other) { swap(*this, other); return *this; }

void http_n::Response::finalize() {
    validateMembers();
    this->setStartLine(std::format("{} {}", this->version()->name(), m_status.toString()));
}

bool http_n::Response::hasChangedSinceLastBuild() const {
    // If m_lastBuild is nullptr, build() was never called and since this
    // is not a static function, we are guaranteed that an object
    // has been created, therefore the response has changed.
    if (not m_lastBuild)
        return true;

    return *m_lastBuild != memento_n::Response(*this);
}

void http_n::Response::initFromStartLineInformation(const startLineInformation_t& information) {
    m_status = Status_s(static_cast<Code>(std::stoi(information[1])));
}

void http_n::Response::parse(std::string_view stringResponse) {
    const startLineInformation_t& responseInfo = this->processMessage(stringResponse);
    initFromStartLineInformation(responseInfo);
}

http_n::Response& http_n::Response::set(std::string_view stringResponse) {
    parse(stringResponse);
    return *this;
}

http_n::Response& http_n::Response::setStatus(Code code) {
    m_status = code;
    return *this;
}

network_n::Status_s http_n::Response::status() const {
    return m_status;
}

void http_n::Response::updateLastBuild() {
    m_lastBuild = std::make_unique<memento_n::Response>(*this);
}

void http_n::Response::validateMembers() const {
    if (m_status.code() == Code::NONE)  throw InvalidArgument("Cannot be empty", "Status");
}