#include <core/str/join.hpp>


std::string join(const std::vector<std::string>& elems, std::string_view delim) {
    std::string joined;

    for (size_t i = 0; i < elems.size(); i++) {
        if (i != 0)
            joined += delim;

        joined += elems[i];
    }

    return joined;
}