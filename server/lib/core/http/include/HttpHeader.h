#pragma once

#include <format>
#include <iostream>
#include <unordered_map>

#include <HttpMethod.h>
#include <Time.h>

const std::string USER_AGENT = "Devola / 1.0";
const std::string PROTOCOL = "HTTP / 1.1";
const std::string CONNECTION_TYPE = "keep - alive";


class HttpHeader {
public:
    std::string Header() const;

    static HttpHeader Build(HttpMethod_e Method, std::string Path, std::string Host, std::string ContentType, std::string ContentLength /* In Bytes */);

    friend std::ostream& operator<<(std::ostream& os, const HttpHeader& obj) { return os << obj.Header(); }
protected:
    HttpHeader(HttpMethod_e Method, std::string Path, std::string Host, std::string ContentType, std::string ContentLength /* In Bytes */);

private:
    std::string FormattedHeader;
    std::unordered_map<HttpMethod_e, std::string> MethodsToString = { { HttpMethod_e::HTTP_DELETE,  "DELETE" },
                                                                      { HttpMethod_e::HTTP_GET,     "GET" },
                                                                      { HttpMethod_e::HTTP_HEAD,    "HEAD" },
                                                                      { HttpMethod_e::HTTP_OPTIONS, "OPTIONS" },
                                                                      { HttpMethod_e::HTTP_PATCH,   "PATCH" },
                                                                      { HttpMethod_e::HTTP_POST,    "POST" },
                                                                      { HttpMethod_e::HTTP_PUT,     "PUT" } };
};

