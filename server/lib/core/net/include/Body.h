#pragma once

#include <Net.h>
#include <string>


const unsigned int SPACES_FOR_INDENT = 4;

template<typename T>
class Body_c : public Net_n::Body_i<T> {
public:
    ~Body_c() = default;

    T Get() const override { return m_Body; }

    std::string ToString() const override {
        if constexpr (std::is_same_v<T, nlohmann::json>)
            return m_Body.dump(SPACES_FOR_INDENT);
        return m_Body;
    }

protected:
    T m_Body;
    std::string m_RawBody;
};