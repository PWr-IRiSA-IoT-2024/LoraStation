#pragma once

#include "Dht.hpp"
#include "Sgp30.hpp"

class Station
{
public:
    static constexpr auto minimalIntervalMs = Dht::minimalIntervalMs;
    static constexpr auto startupDelayMs = 15000;

    struct Measurement
    {
        uint16_t voc;
        uint16_t co2;
        int8_t temperature;
        int8_t humidity;
    };

    explicit Station(uint8_t dhtPin) noexcept
        : _dht {dhtPin}
    {
    }

    auto begin() noexcept -> bool;
    auto setInterval(uint32_t millis) noexcept -> bool;
    auto areNewMeasurementsReady() noexcept -> bool;
    auto getLastMeasurement() noexcept -> const Measurement&;

private:
    auto performMeasurement() noexcept -> bool;

    Dht _dht;
    Sgp30 _sgp;
    Measurement _lastMeasurement {};
    uint32_t _interval = minimalIntervalMs;
    uint32_t _lastMeasurementTime = 0;
    uint32_t _stationUpTime = 0;
};