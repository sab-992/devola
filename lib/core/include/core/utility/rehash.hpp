#pragma once

#include <cstdint>
#include <unordered_map>


namespace
{
    constexpr uint8_t MAX_BUCKET_COUNT_MULTIPLIER = 4;
    constexpr uint8_t MIN_BUCKET_COUNT_MULTIPLIER = 2;
}

template <typename Key, typename T>
void rehashIfNeeded(std::unordered_map<Key, T>& map) {
    const size_t size = map.size();
    if (map.bucket_count() > size * MAX_BUCKET_COUNT_MULTIPLIER)
        map.rehash(size * MIN_BUCKET_COUNT_MULTIPLIER);
}