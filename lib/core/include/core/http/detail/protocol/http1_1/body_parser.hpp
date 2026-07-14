#pragma once

#include <core/exception.hpp>
#include <core/http/detail/protocol/http1_1/headers_parser.hpp>
#include <core/network/interface/body_parser.hpp>
#include <core/network/network.hpp>
#include <core/str/hex.hpp>
#include <core/str/trim.hpp>
#include <core/utility/singleton.hpp>
#include <string>


namespace http_n
{
    namespace protocol_n
    {
        namespace http1_1_n
        {
            class BodyParser : public network_n::protocol_n::BodyParser_i, public Singleton<BodyParser> {
                using Body = network_n::Body;
                using Headers = network_n::Headers;

            public:
                BodyParser(const Singleton<BodyParser>::Private_s&);
                ~BodyParser() = default;

                bool operator==(const BodyParser_i& other) const override;

                std::vector<std::string> build(const Headers& headers, const Body& body) const override;
                std::string parse(const Headers& headers, std::string_view stringBody) const override;

            private:
                const std::string nextLine = "\r\n";

                std::vector<std::string> chunk(const Headers& headers, std::string_view mergedBody) const;

                size_t getChunkSize(std::string_view message, size_t startPos=0) const;

                std::string merge(std::string_view rawBody) const;
            };
        }
    }
}