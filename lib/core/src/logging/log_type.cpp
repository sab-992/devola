#include <core/logging/log_type.h>


rang::fg LogType::m_color;

LogType::LogType(rang::fg color) {
    m_color = color;
}

std::unique_ptr<DisplayColor_i> LogType::debug() {
    return std::move(std::make_unique<LogType>(rang::fg::black));
}

std::unique_ptr<DisplayColor_i> LogType::error() {
    return std::move(std::make_unique<LogType>(rang::fg::red));
}

std::unique_ptr<DisplayColor_i> LogType::info() {
    return std::move(std::make_unique<LogType>(rang::fg::blue));
}


std::unique_ptr<DisplayColor_i> LogType::special() {
    return std::move(std::make_unique<LogType>(rang::fg::magenta));
}

std::unique_ptr<DisplayColor_i> LogType::warning() {
    return std::move(std::make_unique<LogType>(rang::fg::yellow));
}

rang::fg LogType::color() {
    return m_color;
}