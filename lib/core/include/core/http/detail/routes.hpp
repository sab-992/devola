#pragma once

#include <asio.hpp>
#include <core/http/detail/session.hpp>
#include <core/http/request.hpp>
#include <core/http/response.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>


using pathParams_t = std::unordered_map<std::string, std::string>;
using handler_t = std::function<asio::awaitable<http_n::Response>(const http_n::server_n::Session&, const http_n::Request&, const pathParams_t&)>;
using boundHandler_t = std::function<asio::awaitable<http_n::Response>(const http_n::server_n::Session&, const http_n::Request&)>;

namespace http_n
{
    namespace server_n
    {
        class RadixRouter {
            struct Node {
                handler_t handler = nullptr;
                bool isParam = false;
                std::shared_ptr<Node> paramChild = nullptr;
                std::string paramName;
                std::string segment;
                std::unordered_map<std::string, std::shared_ptr<Node>> staticChildren;
            };

        public:
            void addRoute(std::string_view routeTemplate, handler_t handler);
            boundHandler_t route(std::string_view requestPath) const;

        private:
            std::shared_ptr<Node> m_root = std::make_shared<Node>();

            std::vector<std::string_view> tokenize(std::string_view path) const;
        };
    }
}