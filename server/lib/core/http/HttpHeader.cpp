#include <HttpHeader.h>


HttpHeader::HttpHeader(HttpMethod_e Method, std::string Path, std::string Host, std::string ContentType, std::string ContentLength /* In Bytes */) {
    Time Clock;
    FormattedHeader = std::format("{} {} {}\r\nHost: {}\r\nContent-Type: {}\r\nContent-Length: {}\r\nUser-Agent: {}\r\nConnection: {}\r\nDate: {}\r\n",
			                      MethodsToString[Method], Path, PROTOCOL, Host, ContentType, ContentLength, USER_AGENT, CONNECTION_TYPE, Clock.ToUTC());
}

HttpHeader HttpHeader::Build(HttpMethod_e Method, std::string Path, std::string Host, std::string ContentType, std::string ContentLength /* In Bytes */) {
    return HttpHeader(Method, Path, Host, ContentType, ContentLength);
}

std::string HttpHeader::Header() const {
    return FormattedHeader;
}