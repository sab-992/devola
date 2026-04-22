#pragma once

#include <core/exception.h>
#include <core/http/detail/settings.h>
#include <core/network/detail/message.h>
#include <core/network/network.h>
#include <vector>


namespace http_n {
    template <typename T>
    class Response : public network_n::Message<Response<T>, T> {
    public:
        Response() : network_n::Message<Response<T>, T>(http_n::DEFAULT_PROTOCOL) {}
        Response(const Response<T>& other) : network_n::Message<Response<T>, T>(other) { m_status = other.m_status; }
        Response(Response<T>&& other) : network_n::Message<Response<T>, T>(std::move(other)) { m_status = std::move(other.m_status); }

        ~Response() {}

        Response<T>& operator=(Response<T> other) {
            swap(*this, other);
            return *this;
        }

        Response<T>& set(const std::string& stringResponse) {
            parse(stringResponse);
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

    protected:
        void finalize() override {
            validateMembers();
            this->setStartLine(std::format("{} {}", this->getProtocol()->name(), m_status.toString()));
        }

        void updateLastBuild() override {}

    private:
        network_n::Status_s m_status;

        void parse(const std::string& stringResponse) {
            const std::unordered_map<std::string, std::string> responseInfo = this->processMessage(stringResponse);
            m_status = network_n::Status_s(static_cast<network_n::Code>(std::stoi(responseInfo.at("code"))));
        }

        void validateMembers() const {
            if (m_status.code() == network_n::Code::NONE)  throw InvalidArgument("Cannot be empty", "Status");
            if (this->m_protocol == network_n::protocol_n::Protocol::NONE) throw InvalidArgument("Cannot be NONE", "Protocol");
        }
    };
}