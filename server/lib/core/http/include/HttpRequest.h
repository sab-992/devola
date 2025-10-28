#pragma once

#include <HttpBody.h>
#include <HttpCommon.h>
#include <HttpHeaders.h>
#include <HttpMessage.h>
#include <memory>
#include <string>


template<typename T>
class HttpRequest_i : public HttpMessage_i<T> {};

template<typename T>
class HttpRequest : public HttpRequest_i<T>, public HttpMessage_c<T> {
public:
    static std::unique_ptr<HttpRequest_i<T>> Create(std::string RawRequest) {
        return std::move(std::unique_ptr<HttpRequest<T>>(new HttpRequest<T>(RawRequest)));
    }

    template<typename U = T>
    static std::unique_ptr<HttpRequest_i<T>> Create(std::string Method, std::string Path, const Http_n::Endpoint& Endpoint, HeadersUMap_t HeadersMap, U&& Body = T{}) {
        return std::move(std::unique_ptr<HttpRequest<T>>(new HttpRequest<T>(Method, Path, Endpoint, HeadersMap, Body)));
    }

    std::string AsString() const override { return HttpMessage_c<T>::AsString(); };
    Http_n::Endpoint Endpoint() const override { return HttpMessage_c<T>::Endpoint(); };
    T Body() const override { return HttpMessage_c<T>::Body(); }; 
    std::string Headers() const override { return HttpMessage_c<T>::Headers(); };
    HeadersUMap_t HeadersMap() const override { return HttpMessage_c<T>::HeadersMap(); };
    std::string Method() const override { return HttpMessage_c<T>::Method(); };
    std::string Path() const override { return HttpMessage_c<T>::Path(); };
    std::string Raw() const override { return HttpMessage_c<T>::Raw(); };

private:
    HttpRequest(std::string RawRequest) {
        std::pair<std::string, std::string> SeparatedRequest = Http_n::HttpMessageSplitter::Split(RawRequest);
        HttpMessage_c<T>::m_Headers = std::make_unique<HttpHeaders>(SeparatedRequest.first);
        HttpMessage_c<T>::m_Body = std::make_unique<HttpBody<T>>(SeparatedRequest.second);
    };

    template<typename U = T>
    HttpRequest(std::string Method, std::string Path, const Http_n::Endpoint& Endpoint, HeadersUMap_t HeadersMap, U&& Body = T{}) {
        HttpMessage_c<T>::m_Headers = std::make_unique<HttpHeaders>(Method, Path, Endpoint, HeadersMap);
        HttpMessage_c<T>::m_Body = std::make_unique<HttpBody<T>>(std::forward<U>(Body));
    }
};