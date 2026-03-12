// #pragma once

// #include <asio.hpp>
// #include <core/http/detail/request.h>
// #include <core/http/detail/response.h>
// #include <core/http/detail/settings.h>
// #include <core/network/network.h>
// #include <core/utils/converter.h>
// #include <format>
// #include <nlohmann/json.hpp>


// // TODO: Make it singleton
// // TODO: Incorporate in http_n
// // TODO: Change name to client and file name to http-client
// // TODO: Mess around with asio's async feature
// template<typename T>
// class Http {
// public:
//     static std::unique_ptr<http_n::Response_i<std::string>> del(std::string apiEndpoint, const network_n::Endpoint& networkEndpoint) {
//         return http_n::Response<std::string>::create("");
//     }

//     static std::unique_ptr<http_n::Response_i<T>> get(std::string apiEndpoint, const network_n::Endpoint& networkEndpoint) {
//         // TODO change Accept(T()) to somehting else so no need to create T().
//         HeadersUMap_t headersMap = { { "Accept",     accept(T()) },
//                                      { "User-Agent", http_n::USER_AGENT },
//                                      { "Connection", http_n::CLOSE_CONNECTION } };
//         asio::ip::tcp::socket socket(send(http_n::Request<T>::create("GET", apiEndpoint, networkEndpoint, headersMap), networkEndpoint));
        
//         return http_n::Response<T>::create(receive(socket));
//     }

//     static std::unique_ptr<http_n::Response_i<std::string>> head() { return http_n::Response<std::string>::create(""); }
//     static std::unique_ptr<http_n::Response_i<std::string>> options() { return http_n::Response<std::string>::create(""); }
//     static std::unique_ptr<http_n::Response_i<T>> patch() { return http_n::Response<T>::create(T()); }
//     static std::unique_ptr<http_n::Response_i<T>> post() { return http_n::Response<T>::create(T()); }
//     static std::unique_ptr<http_n::Response_i<T>> put() { return http_n::Response<T>::create(T()); }

// private:
//     inline static asio::io_context m_ioContext;

//     static asio::ip::tcp::socket send(const std::unique_ptr<http_n::Request_i<T>>& request, const network_n::Endpoint& networkEndpoint) {
//         asio::ip::tcp::resolver resolver(m_ioContext);
//         asio::ip::tcp::socket socket(m_ioContext);
//         asio::connect(socket, resolver.resolve(networkEndpoint.host(), std::format("{}", networkEndpoint.port())));

//         asio::write(socket, asio::buffer(Converter<http_n::Request_i<T>>::toString(*request)));
//         return socket;
//     }

//     static std::string receive(asio::ip::tcp::socket& socket) {
//         std::string message;
//         std::array<char, http_n::TCP_WINDOW_SIZE> buffer;
//         std::error_code error;
//         while (size_t bytesRead = socket.read_some(asio::buffer(buffer), error)) {
//             if (error == asio::error::eof)
//                 break;
//             else if (error)
//                 throw std::system_error(error);

//             message.append(buffer.data(), bytesRead);
//         }
//         return message;
//     }

//     static std::string accept(json) { return "application/json"; }
//     static std::string accept(std::string) { return "text/html"; }
// };

