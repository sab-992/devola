#include <database/utility/timestamp.hpp>


std::chrono::time_point<std::chrono::system_clock> fromPGSQLFormat(std::string_view value) {
    return Time::timepoint("%Y-%m-%d %H:%M:%S", value);
}

std::string toPGSQLFormat(const std::chrono::time_point<std::chrono::system_clock>& timepoint) {
    return Time::format("{:%Y-%m-%d %H:%M:%S}", timepoint);
}