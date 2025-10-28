#pragma once

#include <format>
#include <HttpCommon.h>
#include <memory>
#include <string>
#include <stdexcept>

template<typename T>
class HttpMessage_i {
public:
    virtual ~HttpMessage_i() = default;

    virtual std::string AsString() const = 0;
    virtual T Body() const = 0;
    virtual Http_n::Endpoint Endpoint() const = 0;
    virtual std::string Headers() const = 0;
    virtual HeadersUMap_t HeadersMap() const = 0;
    virtual std::string Method() const = 0;
    virtual std::string Path() const = 0;
    virtual std::string Raw() const = 0;
};

template<typename T>
class HttpMessage_c : HttpMessage_i<T> {
public:
    virtual ~HttpMessage_c() = default;

    std::string AsString() const override {
        return std::format("{}\r\n\r\n{}", m_Headers->AsString(), m_Body->AsString());
    }

    T Body() const override { 
        ValidateBodyPtr();

        return m_Body->Body(); 
    };

    Http_n::Endpoint Endpoint() const override { 
        ValidateHeaderPtr();

        return m_Headers->Endpoint();
    };

    std::string Headers() const override { 
        ValidateHeaderPtr();

        return m_Headers->AsString(); 
    };

    HeadersUMap_t HeadersMap() const override {
        ValidateHeaderPtr();

        return m_Headers->Map(); 
    };

    std::string Method() const override { 
        ValidateHeaderPtr();

        return m_Headers->Method();
    };

    std::string Path() const override { 
        ValidateHeaderPtr();
        
        return m_Headers->Path();
    };

    std::string Raw() const override {
        ValidateHeaderPtr();
        ValidateBodyPtr();

        return std::format("{}\r\n\r\n{}", m_Headers->Raw(), m_Body->Raw()); 
    };
protected:
    std::unique_ptr<HttpBody<T>> m_Body = nullptr;
    std::unique_ptr<HttpHeaders> m_Headers = nullptr;

private:
    void ValidateHeaderPtr() const {
        if (m_Headers == nullptr)
            throw std::logic_error("Http message m_Headers is null");
    };

    void ValidateBodyPtr() const {
        if (m_Body == nullptr)
            throw std::logic_error("Http message m_Body is null");
    };
};