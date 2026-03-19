#pragma once

#include <core/network/detail/message.h>
#include <core/network/network.h>


namespace http_n {
    template <typename T>
    class Response : public network_n::Message<Response<T>, T> {
    public:
        Response() { this->m_protocol = http_n::DEFAULT_PROTOCOL; }
        Response(const Response<T>& other) : network_n::Message<Response<T>, T>(other) { m_status = other.m_status; }
        Response(Response<T>&& other) : network_n::Message<Response<T>, T>(std::move(other)) { m_status = std::move(other.m_status); }

        ~Response() {}

        Response<T>& operator=(Response<T> other) {
            swap(*this, other);
            return *this;
        }

        Response<T> build() override {
            validateMembers();
            this->setStartLine(std::format("{} {}", this->protocol()->toString(), m_status.toString()));
            return std::move(*this);
        }
        
        Response<T>& setStatus(network_n::Code code) {
            m_status = code;
            return *this;
        }

        network_n::Status_s status() const {
            return m_status;
        }

        friend void swap(Response<T>& lhs, Response<T>& rhs) {
            using std::swap;

            swap(static_cast<network_n::Message<Response<T>, T>&>(lhs), static_cast<network_n::Message<Response<T>, T>&>(rhs));

            swap(lhs.m_status, rhs.m_status);
        }

    private:
        network_n::Status_s m_status;

        void validateMembers() const {
            if (m_status.code() == network_n::Code::NONE)  throw std::invalid_argument("Response status cannot be empty");
        }
    };
}