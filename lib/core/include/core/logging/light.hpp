#pragma once

#include <core/exception.hpp>
#include <core/utility/converter.hpp>
#include <core/logging/detail/rang.hpp>
#include <core/logging/detail/level.hpp>
#include <core/str/trim.hpp>
#include <core/time/time.hpp>
#include <core/utility/singleton.hpp>
#include <string>
#include <unordered_map>


namespace log_n {
    class Light : public Singleton<Light> {
    private:
        using levelInfo_t = std::pair<std::string, rang::fg>;

    public:
        Light(const Private_s&);
        ~Light();

        template<typename... Args>
        void log(log_n::Level_en level, Args... elements) const {
            if (not m_levels.contains(level))
                throw InvalidArgument("Unhandled", "Log type");

            displayTimestamp();
            displayLevel(level);
            (display(elements), ...);

            std::cout << std::endl;
        }

    private:
        // Spaces after the string version of the log level are there to make sure every message starts at same column of terminal.
        inline static const std::unordered_map<log_n::Level_en, levelInfo_t> m_levels = { { log_n::Level_en::DEBUG,   { "[DEBUG]", rang::fg::black } },
                                                                                          { log_n::Level_en::ERROR,   { "[ERROR]", rang::fg::red } },
                                                                                          { log_n::Level_en::INFO,    { "[INFO] ", rang::fg::blue } },
                                                                                          { log_n::Level_en::SPECIAL, { "[SPEC] ", rang::fg::magenta } },
                                                                                          { log_n::Level_en::WARNING, { "[WARN] ", rang::fg::yellow } } };

        void changeColor(log_n::Level_en level) const;
        void resetColor() const;

        void display(const auto& element) const {
            std::string message = Converter::toString(element);

            if (trim(message).empty()) return;

            std::cout << message << " ";
        }
        void displayLevel(log_n::Level_en level) const;
        void displayTimestamp() const;
    };
}