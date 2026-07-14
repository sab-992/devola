#pragma once

#include <core/http/server.hpp>
#include <core/utility/service.hpp>


class RSS : public http_n::server_n::Basic, public Service<RSS> {
    using Basic = http_n::server_n::Basic;

public:
    RSS(const Private_s&, uint16_t port);
    ~RSS();

    static std::unique_ptr<RSS> create(uint16_t port=443);
};