#pragma once

#include <asio.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <core/http/http.hpp>
#include <core/network/network.hpp>


asio::awaitable<void> authenticate(const http_n::Request& request, const std::unique_ptr<http_n::Http>& http);