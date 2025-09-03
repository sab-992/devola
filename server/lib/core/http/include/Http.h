#pragma once

#include <iostream>

#include <asio.hpp>
#include <HttpHeader.h>
#include <HttpMethod.h>
#include <Time.h>


class Http {
public:
    Http();
    ~Http();

    void Delete();
    void Get();
    void Head();
    void Options();
    void Patch();
    void Post();
    void Put();

private:
    void SendRequest(HttpHeader Header, std::string Body);
};