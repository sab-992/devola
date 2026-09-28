#pragma once

#include <sodium.h>
#include <string>


std::string b64Encode(unsigned char* bytes, size_t size, int variant);