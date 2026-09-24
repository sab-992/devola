#include <core/network/network.hpp>


network_n::Status_s::Status_s(Code code) : m_code(code), m_reason(getReasonFromStatus(code)) {}

bool network_n::Status_s::operator==(const Status_s& other) const {
    return m_code == other.m_code;
}

bool network_n::Status_s::operator==(const Code& code) const {
    return m_code == code;
}

bool network_n::Status_s::operator==(uint16_t code) const {
    return to_underlying(m_code) == code;
}

bool network_n::Status_s::operator!=(const Status_s& other) const {
    return !(*this == other);
}

bool network_n::Status_s::operator!=(const Code& code) const {
    return !(*this == code);
}

bool network_n::Status_s::operator!=(uint16_t code) const {
    return !(*this == code);
}

network_n::Code network_n::Status_s::code() const {
    return m_code;
}

std::string network_n::Status_s::reason() const {
    return m_reason;
}

std::string network_n::Status_s::toString() const {
    return std::format("{} {}", to_underlying(m_code), m_reason);
}