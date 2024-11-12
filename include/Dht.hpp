#pragma once

#include <math.h>
#include <stdint.h>

#include <Array.hpp>

class Dht
{
public:
    struct FloatResult
    {
        float value;

        constexpr auto isOk() const noexcept -> bool
        {
            return !isnanf(value);
        }
    };

    struct IntegerResult
    {
        int8_t value;

        constexpr auto isOk() const noexcept -> bool
        {
            return value > 0;
        }
    };

    static constexpr auto minimalIntervalMs = uint32_t {2000};

    explicit Dht(uint8_t pin) noexcept;
    void begin() noexcept;
    auto readTemperature() noexcept -> FloatResult;
    auto readHumidity() noexcept -> FloatResult;
    auto readIntegerTemperature() noexcept -> IntegerResult;
    auto readIntegerHumidity() noexcept -> IntegerResult;

private:
    auto read() noexcept -> bool;
    auto expectPulse(bool level) const noexcept -> uint32_t;

    static constexpr auto measurementError = float {NAN};
    static constexpr auto dataSize = uint32_t {5};
    static constexpr auto pullTimeUs = uint32_t {40};
    static constexpr auto timeout = UINT32_MAX;

    Array<uint8_t, dataSize> _data {};
    uint32_t _previousTime {};
    uint32_t _maxCyclesToWait;
    uint8_t _pin;
    uint8_t _bit;
    uint8_t _port;
    bool _previousResult = false;
};