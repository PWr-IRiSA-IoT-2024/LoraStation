#include "I2cDevice.hpp"

#include <Wire.h>
#include <stddef.h>
#include <stdint.h>

#include "ConstSpan.hpp"
#include "Span.hpp"

I2cDevice::I2cDevice(uint8_t address) noexcept
    : _address {address}
{
}

auto I2cDevice::begin() noexcept -> bool
{
    if (_isBegun)
    {
        return true;
    }
    Wire.begin();
    _isBegun = true;
    return isDetected();
}

auto I2cDevice::isDetected() const noexcept -> bool
{
    if (!_isBegun)
    {
        return false;
    }
    Wire.beginTransmission(_address);
    return Wire.endTransmission() == 0;
}

auto I2cDevice::end() noexcept -> void
{
    Wire.end();
    _isBegun = false;
}

auto I2cDevice::write(ConstSpan<uint8_t> buffer) const noexcept -> bool
{
    if (buffer.empty())
    {
        return false;
    }

    if (buffer.size() > maxBufferSize)
    {
        return false;
    }

    Wire.beginTransmission(_address);

    if (Wire.write(buffer.data(), static_cast<size_t>(buffer.size())) != buffer.size())
    {
        return false;
    }
    if (Wire.endTransmission() == 0)
    {
        return true;
    }
    return false;
}

auto I2cDevice::read(Span<uint8_t> buffer) const noexcept -> bool
{
    if (buffer.size() > maxBufferSize)
    {
        return false;
    }
    if (Wire.requestFrom(_address, static_cast<uint8_t>(buffer.size())) != buffer.size())
    {
        return false;
    }

    for (auto& byte : buffer)
    {
        byte = static_cast<uint8_t>(Wire.read());
    }

    return true;
}

I2cDevice::~I2cDevice() noexcept
{
    if (_isBegun)
    {
        end();
    }
}
