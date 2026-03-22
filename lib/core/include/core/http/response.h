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

        Response<T>& build() & override {
            finalize();
            return *this;
        }

        Response<T> build() && override {
            finalize();
            return std::move(*this);
        }

        Response<T>& set(std::string raw) {
            parseFrom(raw);
            return *this;
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

        void finalize() {
            validateMembers();
            this->setStartLine(std::format("{} {}", this->protocol()->toString(), m_status.toString()));
        }

        void parseFrom(const std::string& raw) {
            // TODO: Add protocol detection and change it accordingly
            auto [headers, body] = this->protocol()->parse(raw);

            this->m_headers = headers;
            this->m_body = body;

            std::string statusCode = this->protocol()->parseStartLine(this->m_headers.startLine())["code"];
            m_status = network_n::Status_s(static_cast<network_n::Code>(std::stoi(statusCode)));
        }
    };
}