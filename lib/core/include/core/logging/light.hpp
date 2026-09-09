#pragma once

#include <mutex>
#include <core/exception.hpp>
#include <core/utility/converter.hpp>
#include <core/logging/detail/rang.hpp>
#include <core/logging/detail/level.hpp>
#include <core/str/trim.hpp>
#include <core/time/time.hpp>
#include <core/utility/singleton.hpp>
#include <string>
#include <unordered_map>


inline std::mutex coutMutex;

namespace log_n {
    class Light : public Singleton<Light> {
    public:
        Light(const Private_s&);
        ~Light();

        template<typename... Args>
        void log(log_n::Level_en level, Args... elements) const {
            log(level, {}, elements...);
        }

        template<typename... Args>
        void log(log_n::Level_en level, const std::vector<std::string>& information, Args... elements) const {

            #ifndef DEBUG_MODE_ENABLED
            if (level == log_n::Level_en::DEBUG)
                return;
            #endif

            if (not LEVELS_COLOR.contains(level))
                throw InvalidArgument("Unhandled", "Log type");

            std::lock_guard<std::mutex> guard(coutMutex);
            displayTimestamp();
            displayLevel(level);

            if (not information.empty())
                displayExtraInformation(information);

            std::cout << " - ";

            (display(elements), ...);

            std::cout << std::endl;
        }

    private:
        void changeColor(log_n::Level_en level) const;
        void resetColor() const;

        void display(const auto& element) const {
            std::string message = Converter::toString(element);

            if (trim(message).empty()) return;

            std::cout << message << " ";
        }

        void displayExtraInformation(const std::vector<std::string>& information) const;
        void displayLevel(log_n::Level_en level) const;
        void displayTimestamp() const;
    };
}