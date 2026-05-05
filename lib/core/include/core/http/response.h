#pragma once

#include <core/exception.h>
#include <core/http/detail/memento/response.h>
#include <core/http/detail/settings.h>
#include <core/network/detail/message.h>
#include <core/network/network.h>
#include <memory>


namespace http_n
{
    namespace memento_n
    {
        template <typename T>
        class Response;
    }

    template <typename T>
    class Response : public network_n::Message<Response<T>, T> {
    public:
        Response() : network_n::Message<Response<T>, T>(http_n::DEFAULT_PROTOCOL) {}

        Response(const Response<T>& other) : network_n::Message<Response<T>, T>(other) {
            if (other.m_lastBuild)
                m_lastBuild = std::make_unique<memento_n::Response<T>>(*other.m_lastBuild);
            m_status = other.m_status;
        }

        Response(Response<T>&& other) : network_n::Message<Response<T>, T>(std::move(other)) {
            m_lastBuild = std::move(other.m_lastBuild);
            m_status = std::move(other.m_status);
        }

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

            swap(lhs.m_lastBuild, rhs.m_lastBuild);
            swap(lhs.m_status, rhs.m_status);
        }

    protected:
        void finalize() override {
            validateMembers();
            this->setStartLine(std::format("{} {}", this->protocol()->name(), m_status.toString()));
        }

        bool hasChangedSinceLastBuild() const override {
            // If it is nullptr, build() was never called.
            // Since this is not a static function, we are guaranteed that an object has been created,
            // therefore the object has changed.
            if (not m_lastBuild)
                return true;

            return *m_lastBuild == memento_n::Response<T>(*this);
        }

        void updateLastBuild() override { m_lastBuild = std::make_unique<memento_n::Response<T>>(*this); }

    private:
        std::unique_ptr<memento_n::Response<T>> m_lastBuild;
        network_n::Status_s m_status;

        friend class memento_n::Response<T>;

        void parse(const std::string& stringResponse) {
            const startLineInformation_t responseInfo = this->processMessage(stringResponse);
            m_status = network_n::Status_s(static_cast<network_n::Code>(std::stoi(responseInfo[1])));
        }

        void validateMembers() const {
            if (m_status.code() == network_n::Code::NONE)  throw InvalidArgument("Cannot be empty", "Status");
        }
    };
}