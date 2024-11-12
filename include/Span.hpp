#pragma once

#include <stdint.h>

#include "Array.hpp"

template <typename T>
class Span
{
public:
    template <uint32_t N>
    constexpr explicit Span(Array<T, N>& array)
        : _data {array.data},
          _size {array.size()}
    {
    }

    constexpr Span(T* buffer, uint32_t size)
        : _data {buffer},
          _size {size}
    {
    }

    constexpr auto offset(uint32_t offset) const noexcept -> Span<T>
    {
        return {_data + offset, _size - offset};
    }

    constexpr auto subspan(uint32_t offset, uint32_t length) const noexcept -> Span<T>
    {
        return {_data + offset, length};
    }

    constexpr auto data() const noexcept -> const T*
    {
        return _data;
    }

    auto data() noexcept -> T*
    {
        return _data;
    }

    constexpr auto operator[](uint32_t at) const noexcept -> const T&
    {
        return _data[at];  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    auto operator[](uint32_t at) noexcept -> T&
    {
        return _data[at];  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    constexpr auto size() const noexcept -> uint32_t
    {
        return _size;
    }

    constexpr auto empty() const noexcept -> bool
    {
        return _size == 0;
    }

    constexpr auto begin() const -> const T*
    {
        return _data;
    }

    auto begin() -> T*
    {
        return _data;
    }

    constexpr auto end() const -> const T*
    {
        return _data + _size;
    }

    auto end() -> T*
    {
        return _data + _size;
    }

private:
    T* _data;
    uint32_t _size;
};
