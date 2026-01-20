#pragma once

#include <concepts>
#include <core/network/interface/headers.h>


namespace network_n
{
    template <typename U>
    concept IsHeader_cpt = std::derived_from<U, network_n::Headers_i>;
}