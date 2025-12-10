#pragma once

#include <HttpBody.h>
#include <NetCommon.h>
#include <HttpHeaders.h>
#include <HttpMessage.h>
#include <memory>
#include <string>

template<typename T>
class HttpResponse_i : public HttpMessage_i<T> {
public:
    virtual Net_n::Status Status() const = 0;
};

template<typename T>
class HttpResponse : public HttpResponse_i<T>, public HttpMessage_c<T> {
public:
    static std::unique_ptr<HttpResponse_i<T>> Create(std::string RawResponse) {
        return std::unique_ptr<HttpResponse<T>>(new HttpResponse<T>(RawResponse));
    }

    std::string ToString() const override { return HttpMessage_c<T>::ToString(); };
    T Body() const override { return HttpMessage_c<T>::Body(); };
    Net_n::Endpoint Endpoint() const override { return HttpMessage_c<T>::Endpoint(); };
    std::string Headers() const override { return HttpMessage_c<T>::Headers(); };
    HeadersUMap_t HeadersMap() const override { return HttpMessage_c<T>::HeadersMap(); };
    std::string Method() const override { return HttpMessage_c<T>::Method(); };
    std::string Path() const override { return HttpMessage_c<T>::Path(); };
    std::string Raw() const override { return HttpMessage_c<T>::Raw(); };

    Net_n::Status Status() const override { return HttpMessage_c<T>::m_Headers->Status(); };

private:
    HttpResponse(std::string RawResponse) {
        std::pair<std::string, std::string> SeparatedResponse = HttpMessage_c<T>::Split(RawResponse);
        HttpMessage_c<T>::m_Headers = std::make_unique<HttpHeaders>(SeparatedResponse.first);
        HttpMessage_c<T>::m_Body = std::make_unique<HttpBody<T>>(SeparatedResponse.second,
                                                                 HttpMessage_c<T>::m_Headers->TransferEncoding() == Http_n::CHUNKED /* IsChunked */);

        HttpMessage_c<T>::ValidateMemberVariables();
    };
};