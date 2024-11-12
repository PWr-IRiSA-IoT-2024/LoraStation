#pragma once

#include <stdint.h>

#include "SensorManager.hpp"

class MeasurementQueue
{
public:
    explicit MeasurementQueue(const SensorManager& station) noexcept;

    auto getNextMeasurement() noexcept -> uint8_t;

private:
    enum class Type : uint8_t
    {
        Co2 = 0,
        Voc,
        Temperature,
        Humidity,
        MaxType
    };

    static constexpr auto maxValue = uint8_t {0x1F};

    static constexpr auto toInt(Type type) noexcept -> uint8_t
    {
        return static_cast<uint8_t>(type);
    }

    static auto castCo2To5bits(uint16_t co2) noexcept -> uint8_t;
    static auto castVocTo5bits(uint16_t voc) noexcept -> uint8_t;
    static auto castTemperatureTo5Bits(float temperature) noexcept -> uint8_t;
    static auto castHumidityTo5Bits(uint8_t humidity) noexcept -> uint8_t;

    auto addTypeToValue(uint8_t value) const noexcept -> uint8_t;
    auto getMeasurement() noexcept -> uint8_t;

    const SensorManager& _station;
    uint32_t _requestId = 0;
};