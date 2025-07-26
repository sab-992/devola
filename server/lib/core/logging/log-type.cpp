#include <log-type.h>

rang::fg LogType::m_Color;

LogType::LogType(rang::fg Color) {
    m_Color = Color;
}

std::unique_ptr<DisplayColor_i> LogType::Debug() {
    return std::move(std::make_unique<LogType>(rang::fg::magenta));
}

std::unique_ptr<DisplayColor_i> LogType::Error() {
    return std::move(std::make_unique<LogType>(rang::fg::red));
}

std::unique_ptr<DisplayColor_i> LogType::Info() {
    return std::move(std::make_unique<LogType>(rang::fg::blue));
}

std::unique_ptr<DisplayColor_i> LogType::Warning() {
    return std::move(std::make_unique<LogType>(rang::fg::yellow));
}

rang::fg LogType::Color() {
    return m_Color;
}