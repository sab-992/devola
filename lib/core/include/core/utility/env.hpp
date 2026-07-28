#pragma once


#include <core/file/service.hpp>
#include <core/str/split.hpp>
#include <core/str/trim.hpp>
#include <string>


using env_t = std::unordered_map<std::string, std::string>;

env_t env(std::string_view path);