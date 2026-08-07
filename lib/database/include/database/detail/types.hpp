#pragma once

#include <string>
#include <unordered_map>


namespace database_n
{
    class Value;
    using record_t = std::unordered_map<std::string, Value>;
}