#pragma once

#include <concepts>
#include <core/network/interface/body.h>


namespace network_n
{
    template<typename U, typename T>
    concept IsBody_cpt = std::derived_from<U, network_n::Body_i<T>>;
}