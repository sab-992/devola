#include <utility/url.hpp>


std::pair<std::string_view, std::string_view> parseURL(std::string_view url) {
    const std::string END_OF_PREFIX_TOKEN = "://";
    std::size_t start = url.find(END_OF_PREFIX_TOKEN);
    start = (start == std::string::npos) ? 0 : start + END_OF_PREFIX_TOKEN.size();

    std::size_t pathStart = url.find_first_of("/?#", start);
    return { url.substr(start, pathStart - start), pathStart == std::string::npos ? "/" : url.substr(pathStart) };
}