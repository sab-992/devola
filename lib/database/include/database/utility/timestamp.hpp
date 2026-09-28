#pragma once

#include <core/time.hpp>


std::chrono::time_point<std::chrono::system_clock> fromPGSQLFormat(std::string_view value);
std::string toPGSQLFormat(const std::chrono::time_point<std::chrono::system_clock>& timepoint);