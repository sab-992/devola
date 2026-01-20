#pragma once

#include <core/network/detail/body.h>
#include <core/network/network.h>
#include <core/str/trim.h>
#include <format>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>


namespace websocket_n
{
    template<typename T>
    class Body : public network_n::Body_c<T> {
    public:
        Body() {}

        static websocket_n::Body<T> parse(std::string rawBody) {
            if constexpr (std::is_same_v<T, nlohmann::json>)
                return websocket_n::Body<T>(json::parse(rawBody));
            return websocket_n::Body<T>(rawBody);
        }

        static websocket_n::Body<T> build(T messageBody) {
            return websocket_n::Body<T>(messageBody);
        }

    private:
        Body(T messageBody) {
            network_n::Body_c<T>::m_body = messageBody;
        }
    };
}