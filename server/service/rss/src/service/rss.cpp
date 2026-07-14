#include <service/rss.hpp>


RSS::RSS(const Private_s&, uint16_t port) : Basic("RSS", port) {}

RSS::~RSS() {}

std::unique_ptr<RSS> RSS::create(uint16_t port) {
    return std::make_unique<RSS>(Private_s(), port);
}