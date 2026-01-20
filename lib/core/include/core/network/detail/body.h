#pragma once

#include <core/network/interface/body.h>
#include <string>


const unsigned int SPACES_FOR_INDENT = 4;

namespace network_n
{
    template<typename T>
    class Body_c : public network_n::Body_i<T> {
    public:
        ~Body_c() = default;

        T get() const override { return m_body; }

    protected:
        T m_body;

        Body_c() : m_body(T{}) {}

        std::string toString() const override {
            if constexpr (std::is_same_v<T, nlohmann::json>)
                return m_body.dump(SPACES_FOR_INDENT);
            return m_body;
        }
    };
}