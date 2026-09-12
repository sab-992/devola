#pragma once

#include <core/exception.hpp>
#include <core/str/trim.hpp>
#include <core/str/split.hpp>
#include <core/utility/interface/builder.hpp>
#include <core/utility/string_convertible.hpp>
#include <string>


namespace network_n
{
    class Cookie : public Builder_i<Cookie>, public StringConvertible {
    public:
        Cookie();
        Cookie(std::string_view cookie);

        ~Cookie() = default;

        Cookie& build() & override;
        Cookie build() && override;

        bool isBrowserOnly() const;
        std::chrono::seconds maxAge() const;
        const std::string& name() const;
        const std::string& path() const;
        const std::string& value() const;

        Cookie& setMaxAge(std::chrono::seconds ttl);
        Cookie& setName(std::string_view name);
        Cookie& setPath(std::string_view path);
        Cookie& setRestrictionToBrowser(bool isBrowserOnly);
        Cookie& setValue(std::string_view value);


        std::string buildForTransmission() const;
        void parse(std::string_view cookie);
        std::string toString() const override;

    private:
        bool m_isBrowserOnly = true;
        std::chrono::seconds m_maxAge = std::chrono::seconds(0);
        std::string m_name;
        std::string m_path = "/";
        std::string m_value;
    };
}