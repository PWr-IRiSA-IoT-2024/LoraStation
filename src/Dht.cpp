#include "Dht.hpp"

#include <Arduino.h>
#include <limits.h>
#include <stdint.h>

#include "Array.hpp"
#include "InterruptLock.hpp"

Dht::Dht(uint8_t pin) noexcept
    : _maxCyclesToWait {microsecondsToClockCycles(1000)},
      _pin {pin},
      // NOLINTNEXTLINE
      _bit {digitalPinToBitMask(pin)},
      // NOLINTNEXTLINE
      _port {digitalPinToPort(pin)}
{
}

auto Dht::begin() noexcept -> void
{
    pinMode(_pin, INPUT_PULLUP);
    _previousTime = millis() - minimalIntervalMs;
}

auto Dht::readIntegerTemperature() noexcept -> IntegerResult
{
    if (read())
    {
        return {static_cast<int8_t>(_data[2])};
    }
    return {-1};
}

auto Dht::readIntegerHumidity() noexcept -> IntegerResult
{
    if (read())
    {
        return {static_cast<int8_t>(_data[0])};
    }
    return {-1};
}

auto Dht::readTemperature() noexcept -> FloatResult
{
    if (read())
    {
        const auto sign = static_cast<uint8_t>(_data[3] & 0x80U);
        const auto integerPart = static_cast<uint32_t>(_data[2]);
        const auto decimalPart = static_cast<float>(_data[3] & 0x7FU) / 10.F;
        const auto absoluteValue = static_cast<float>(integerPart) + decimalPart;

        return {(sign > 1 ? -1.F : 1.F) * absoluteValue};
    }
    return {measurementError};
}

auto Dht::readHumidity() noexcept -> FloatResult
{
    if (read())
    {
        const auto integerPart = _data[0];
        const auto decimalPart = static_cast<float>(_data[1]) / 10.F;
        return {static_cast<float>(integerPart) + decimalPart};
    }
    return {measurementError};
}

auto Dht::read() noexcept -> bool
{
    const auto currentTime = millis();
    if (currentTime - _previousTime < minimalIntervalMs)
    {
        return _previousResult;
    }

    _previousTime = currentTime;
    _data = {};

    pinMode(_pin, INPUT_PULLUP);

    static constexpr auto initialDelayMs = uint32_t {1};
    delay(initialDelayMs);

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    static constexpr auto pullDownDelayMs = uint32_t {20};
    delay(pullDownDelayMs);

    Array<uint32_t, 80> cycles {};
    {
        pinMode(_pin, INPUT_PULLUP);
        delayMicroseconds(pullTimeUs);

        const InterruptLock lock {};

        if (expectPulse(LOW) == timeout)
        {
            _previousResult = false;
            return _previousResult;
        }
        if (expectPulse(HIGH) == timeout)
        {
            _previousResult = false;
            return _previousResult;
        }

        for (auto i = uint32_t {}; i < cycles.size(); i += 2)
        {
            cycles[i] = expectPulse(LOW);
            cycles[i + 1] = expectPulse(HIGH);
        }
    }

    for (auto i = uint32_t {}; i < cycles.size() / 2; i++)
    {
        const auto lowCycles = cycles[2 * i];
        const auto highCycles = cycles[(2 * i) + 1];

        if ((lowCycles == timeout) || highCycles == timeout)
        {
            _previousResult = false;
            return _previousResult;
        }

        _data[i / CHAR_BIT] = static_cast<uint8_t>(_data[i / CHAR_BIT] << uint32_t {1});
        if (highCycles > lowCycles)
        {
            _data[i / CHAR_BIT] = static_cast<uint8_t>(_data[i / CHAR_BIT] | uint8_t {1});
        }
    }

    _previousResult =
        _data[4] == ((uint32_t {_data[0]} + uint32_t {_data[1]} + uint32_t {_data[2]} + uint32_t {_data[3]}) & 0xFFU);

    return _previousResult;
}

auto Dht::expectPulse(bool level) const noexcept -> uint32_t
{
    auto count = uint16_t {};
    const auto portState = level ? _bit : uint8_t {0};
    //NOLINTNEXTLINE
    while ((*portInputRegister(_port) & _bit) == portState)
    {
        if (count++ > _maxCyclesToWait)
        {
            return timeout;
        }
    }

    return count;
}
