#pragma once

#include <stdint.h>

#include "Dht.hpp"
#include "Sgp30.hpp"

class SensorManager
{
public:
    static constexpr auto minimalIntervalMs = Dht::minimalIntervalMs;
    static constexpr auto startupDelayMs = 15000;

    struct Measurement
    {
        uint16_t voc;
        uint16_t co2;
        float temperature;
        uint8_t humidity;
    };

    explicit SensorManager(uint8_t dhtPin) noexcept
        : _dht {dhtPin}
    {
    }

    auto begin() noexcept -> bool;
    auto setInterval(uint32_t millis) noexcept -> bool;
    auto areNewMeasurementsReady() const noexcept -> bool;
    auto getLastMeasurement() const noexcept -> const Measurement&;
    auto getVoc() const noexcept -> uint16_t;
    auto getCo2() const noexcept -> uint16_t;
    auto getTemperature() const noexcept -> float;
    auto getHumidity() const noexcept -> uint8_t;
    auto performMeasurementIfReady() noexcept -> bool;

private:
    static auto getHumidity(float temperature, uint8_t relativeHumidity) -> uint8_t;
    auto performMeasurement() noexcept -> bool;

    Dht _dht;
    Sgp30 _sgp;
    Measurement _lastMeasurement {};
    uint32_t _interval = minimalIntervalMs;
    uint32_t _lastMeasurementTime = 0;
    uint32_t _stationUpTime = 0;
};