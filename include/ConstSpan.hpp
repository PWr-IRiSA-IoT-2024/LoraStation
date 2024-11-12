#pragma once

#include <stdint.h>

#include "Array.hpp"
#include "Span.hpp"

template <typename T>
struct ConstSpan
{
public:
    template <uint32_t N>
    constexpr explicit ConstSpan(const Array<T, N>& array) noexcept
        : _data {array.data},
          _size {array.size()}
    {
    }

    template <uint32_t N>
    constexpr explicit ConstSpan(const Array<T, N>&& array) = delete;

    constexpr ConstSpan(const T* buffer, uint32_t size) noexcept
        : _data {buffer},
          _size {size}
    {
    }

    constexpr explicit ConstSpan(Span<T> span) noexcept
        : _data {span.data()},
          _size {span.size()}
    {
    }

    constexpr explicit ConstSpan(const T& value) noexcept
        : _data {&value},
          _size {1}
    {
    }

    constexpr explicit ConstSpan(const T&& value) = delete;

    constexpr auto offset(uint32_t offset) const noexcept -> ConstSpan<T>
    {
        return {_data + offset, _size - offset};
    }

    constexpr auto subspan(uint32_t offset, uint32_t length) const noexcept -> ConstSpan<T>
    {
        return {_data + offset, length};
    }

    constexpr auto data() const noexcept -> const T*
    {
        return _data;
    }

    constexpr auto operator[](uint32_t at) const noexcept -> const T&
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

    constexpr auto begin() const noexcept -> const T*
    {
        return _data;
    }

    constexpr auto end() const noexcept -> const T*
    {
        return _data + _size;
    }

private:
    const T* _data;
    uint32_t _size;
};