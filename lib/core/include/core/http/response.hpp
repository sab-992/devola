#pragma once

#include <core/exception.hpp>
#include <core/http/detail/memento/response.hpp>
#include <core/http/detail/settings.hpp>
#include <core/network/detail/message.hpp>
#include <core/network/network.hpp>
#include <memory>


namespace http_n
{
    namespace memento_n { class Response; }

    class Response : public network_n::Message<Response> {
        using Body = network_n::Body;
        using Headers = network_n::Headers;
        template<typename T>
        using Message = network_n::Message<T>;
        using Status_s = network_n::Status_s;
        using Code = network_n::Code;

    public:
        Response();
        Response(Headers headers, Body body);
        Response(const Response& other);
        Response(Response&& other) = default;
        ~Response() = default;

        Response& operator=(Response other);

        Response& set(std::string_view stringResponse);
        Response& setStatus(Code code);

        Status_s status() const;

        friend void swap(Response& lhs, Response& rhs) {
            using std::swap;

            Message<Response>::swapMessages(&lhs, &rhs);

            swap(lhs.m_lastBuild, rhs.m_lastBuild);
            swap(lhs.m_status, rhs.m_status);
        }

    protected:
        bool hasChangedSinceLastBuild() const override;

        void finalize() override;
        void updateLastBuild() override;

    private:
        std::unique_ptr<memento_n::Response> m_lastBuild;
        Status_s m_status;

        friend class memento_n::Response;

        void initFromStartLineInformation(const startLineInformation_t& information);

        void parse(std::string_view stringResponse);

        void validateMembers() const;
    };
}