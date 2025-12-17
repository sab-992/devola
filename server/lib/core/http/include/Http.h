#pragma once

#include <asio.hpp>
#include <Net.h>
#include <HttpRequest.h>
#include <HttpResponse.h>
#include <HttpSettings.h>
#include <nlohmann/json.hpp>


template<typename T>
class Http {
public:
    static std::unique_ptr<Http_n::Response_i<std::string>> Delete(std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint) {
        return Http_n::Response<std::string>::Create("");
    }

    static std::unique_ptr<Http_n::Response_i<T>> Get(std::string APIEndpoint, const Net_n::NetworkEndpoint& NetworkEndpoint) {
        // TODO change Accept(T()) to somehting else so no need to create T().
        HeadersUMap_t HeadersMap = { { "Accept",  Accept(T()) },
                                     { "User-Agent", Http_n::USER_AGENT },
                                     { "Connection", Http_n::CLOSE_CONNECTION } };
        asio::ip::tcp::socket Socket(Send(Http_n::Request<T>::Create("GET", APIEndpoint, NetworkEndpoint, HeadersMap)));
        
        return Http_n::Response<T>::Create(Receive(Socket));
    }

    static std::unique_ptr<Http_n::Response_i<std::string>> Head() { return Http_n::Response<std::string>::Create(""); }
    static std::unique_ptr<Http_n::Response_i<std::string>> Options() { return Http_n::Response<std::string>::Create(""); }
    static std::unique_ptr<Http_n::Response_i<T>> Patch() { return Http_n::Response<T>::Create(T()); }
    static std::unique_ptr<Http_n::Response_i<T>> Post() { return Http_n::Response<T>::Create(T()); }
    static std::unique_ptr<Http_n::Response_i<T>> Put() { return Http_n::Response<T>::Create(T()); }

private:
    inline static asio::io_context m_IOCtx;

    static asio::ip::tcp::socket Send(const std::unique_ptr<Http_n::Request_i<T>>& Request) {
        asio::ip::tcp::resolver Resolver(m_IOCtx);
        asio::ip::tcp::socket Socket(m_IOCtx);
        asio::connect(Socket, Resolver.resolve(Request->NetworkEndpoint().Host(), std::format("{}", Request->NetworkEndpoint().Port())));

        asio::write(Socket, asio::buffer(Request->ToString()));
        return Socket;
    }

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
    }

    static std::string Accept(json) { return "application/json"; }
    static std::string Accept(std::string) { return "text/html"; }
};

