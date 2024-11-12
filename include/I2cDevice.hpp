#pragma once

#include <stdint.h>

#include "ConstSpan.hpp"
#include "Span.hpp"

class I2cDevice
{
public:
    explicit I2cDevice(uint8_t address) noexcept;
    I2cDevice(const I2cDevice& device) = default;
    I2cDevice(I2cDevice&& device) = default;
    auto operator=(const I2cDevice& device) -> I2cDevice& = default;
    auto operator=(I2cDevice&& device) -> I2cDevice& = default;
    ~I2cDevice() noexcept;

    auto begin() noexcept -> bool;
    auto end() noexcept -> void;
    auto isDetected() const noexcept -> bool;

    auto read(Span<uint8_t> buffer) const noexcept -> bool;
    auto write(ConstSpan<uint8_t> buffer) const noexcept -> bool;

private:
    static constexpr auto maxBufferSize = uint32_t {32};

    uint8_t _address;
    bool _isBegun = false;
};