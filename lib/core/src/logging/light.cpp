#include <core/logging/light.hpp>


log_n::Light::Light(const Private_s&) {}

log_n::Light::~Light() {
    resetColor();
};

void log_n::Light::changeColor(log_n::Level_en level) const {
    std::cout << m_levels.at(level).second;
}

void log_n::Light::displayLevel(log_n::Level_en level) const {
    changeColor(level);
    std::cout << m_levels.at(level).first;
    resetColor();
    std::cout << " - ";
}

void log_n::Light::displayTimestamp() const {
    std::cout << std::format("[{}] ", Time::format<std::chrono::seconds>("{:%F %T %Z}", Time::now()));
}

void log_n::Light::resetColor() const {
    std::cout << rang::style::reset;
}