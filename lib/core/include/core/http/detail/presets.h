#pragma once

#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/http/request.h>
#include <core/http/response.h>

namespace http_n {
    // Function names are capitalized because 'delete' is a C++ reserved keyword.
    class Presets {
    public:
        Presets() = delete;
        ~Presets() = default;

        template<typename T>
        class Request {
        public:
            static http_n::Request<T> DELETE() {
                // TODO
                return create().setMethod("DELETE")
                               .setHeader("Connection", CLOSE_CONNECTION);
            }

            static http_n::Request<T> GET() {
                return create().setMethod("GET")
                               .setHeader("Accept", accept())
                               .setHeader("User-Agent", USER_AGENT)
                               .setHeader("Connection", CLOSE_CONNECTION);
            }

            static http_n::Request<T> HEAD() {
                // TODO
                return create().setMethod("HEAD")
                               .setHeader("Connection", CLOSE_CONNECTION);
            }

            static http_n::Request<T> OPTIONS() {
                // TODO
                return create().setMethod("OPTIONS")
                               .setHeader("Connection", CLOSE_CONNECTION);
            }

            static http_n::Request<T> PATCH() {
                // TODO
                return create().setMethod("PATCH")
                               .setHeader("Connection", CLOSE_CONNECTION);
            }

            static http_n::Request<T> POST() {
                // TODO
                return create().setMethod("POST")
                               .setHeader("Connection", CLOSE_CONNECTION);
            }

            static http_n::Request<T> PUT() {
                // TODO
                return create().setMethod("PUT")
                               .setHeader("Connection", CLOSE_CONNECTION);
            }
        private:
            inline static const std::string CLOSE_CONNECTION = "close";
            inline static const std::string USER_AGENT = "Devola/1.0";

            inline static std::string accept() { throw Exception("Not Implemented"); }
            static http_n::Request<T> create() { return http_n::Request<T>().setProtocol(network_n::protocol_n::Protocol::HTTP1_1); }
        };
    };

    template<>
    inline std::string Presets::Request<nlohmann::json>::accept() { return "application/json"; }

    template<>
    inline std::string Presets::Request<std::string>::accept() { return "text/html"; }
}
