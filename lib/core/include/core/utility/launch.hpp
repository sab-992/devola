#pragma once


#include <core/file/service.hpp>


nlohmann::json readConfigJSON(std::string_view path);