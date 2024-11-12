#pragma once

#include <stdint.h>

template <typename T, uint32_t N>
struct Array
{
    T data[N];

    constexpr auto operator[](uint32_t at) const noexcept -> const T&
    {
        return data[at];
    }

    auto operator[](uint32_t at) noexcept -> T&
    {
        return data[at];
    }

    constexpr auto size() const noexcept -> uint32_t
    {
        return N;
    }

    constexpr auto empty() const noexcept -> bool
    {
        return N == 0;
    }
};