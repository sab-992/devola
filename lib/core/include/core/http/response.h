#pragma once

#include <core/network/detail/message.h>
#include <core/network/network.h>


namespace http_n {
    template <typename T>
    class Response : public network_n::Message<Response<T>, T> {
    public:
        Response() {}

        Response<T>& build() override {
            validateMembers();
            this->setStartLine(std::format("{} {} {}", this->m_protocol->toString(), m_status.code(), m_status.reason()));
            return *this;
        }
        
        Response<T>& setStatus(network_n::Code code) {
            m_status = code;
            return *this;
        }

        network_n::Status_s status() const {
            return m_status;
        }

    private:
        network_n::Status_s m_status;

        void validateMembers() {
            if (m_status.code() == network_n::Code::NONE)  throw std::invalid_argument("Response status cannot be empty");
        }
    };
}