#pragma once

#include <asio.hpp>
#include <HttpCommon.h>
#include <HttpRequest.h>
#include <HttpResponse.h>
#include <HttpSettings.h>
#include <nlohmann/json.hpp>


template<typename T>
class Http {
public:
    static std::unique_ptr<HttpResponse_i<std::string>> Delete(std::string Path, const Http_n::Endpoint& Endpoint) {
        return HttpResponse<std::string>::Create("");
    };

    static std::unique_ptr<HttpResponse_i<T>> Get(std::string Path, const Http_n::Endpoint& Endpoint) {
        HeadersUMap_t HeadersMap = { { "Accept",  Accept(T()) },
                                     { "User-Agent", Http_n::USER_AGENT },
                                     { "Connection", Http_n::CLOSE_CONNECTION } };
        asio::ip::tcp::socket Socket(Send(HttpRequest<T>::Create("GET", Path, Endpoint, HeadersMap)));
        
        return HttpResponse<T>::Create(Receive(Socket));
    };

    static std::unique_ptr<HttpResponse_i<std::string>> Head() { return HttpResponse<std::string>::Create(""); };
    static std::unique_ptr<HttpResponse_i<std::string>> Options() { return HttpResponse<std::string>::Create(""); };
    static std::unique_ptr<HttpResponse_i<T>> Patch() { return HttpResponse<T>::Create(T()); };
    static std::unique_ptr<HttpResponse_i<T>> Post() { return HttpResponse<T>::Create(T()); };
    static std::unique_ptr<HttpResponse_i<T>> Put() { return HttpResponse<T>::Create(T()); };

private:
    static asio::ip::tcp::socket Send(const std::unique_ptr<HttpRequest_i<T>>& Request) {
        asio::io_context IOCtx;
        asio::ip::tcp::resolver Resolver(IOCtx);

        asio::ip::tcp::socket Socket(IOCtx);
        asio::connect(Socket, Resolver.resolve(Request->Endpoint().Host, std::format("{}", Request->Endpoint().Port)));

        asio::write(Socket, asio::buffer(Request->AsString()));
        return Socket;
    };

    static std::string Receive(asio::ip::tcp::socket& Socket) {
        std::string Message;
        std::array<char, Http_n::TCP_WINDOW_SIZE> Buffer;
        std::error_code Error;
        while (size_t BytesRead = Socket.read_some(asio::buffer(Buffer), Error)) {
            if (Error == asio::error::eof)
                break;
            else if (Error)
                throw std::system_error(Error);

            Message.append(Buffer.data(), BytesRead);
        }
        return Message;
    };

    static std::string Accept(json) { return "application/json"; };
    static std::string Accept(std::string) { return "text/html"; };
};

