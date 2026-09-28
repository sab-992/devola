#pragma once

#include <core/str/case.hpp>
#include <sstream>
#include <string>


std::string toHex(unsigned long num, bool uppercase=true);
unsigned long fromHex(std::string_view num);