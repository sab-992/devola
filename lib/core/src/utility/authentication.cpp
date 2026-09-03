#include <core/utility/authentication.hpp>


asio::awaitable<void> authenticate(const http_n::Request& request, const std::unique_ptr<http_n::Http>& http) {
    http_n::Request authenticationRequest(request);
    authenticationRequest.setURL("localhost")
                         .setAPIEndpoint("/user/authenticate").build();

    const http_n::Response& response = co_await http->async_receive(co_await http->async_send(authenticationRequest));
    if (response.status() == network_n::Code::UNAUTHORIZED)
        throw InvalidToken("Invalid user token");
    else if (response.status() == network_n::Code::SERVER_ERROR)
        throw Exception("Error during [user] service's user authentication");
}