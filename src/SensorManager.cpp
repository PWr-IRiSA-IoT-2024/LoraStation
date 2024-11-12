#include "SensorManager.hpp"

#include <math.h>
#include <stdint.h>

#include "Arduino.h"

auto SensorManager::getHumidity(float temperature, uint8_t relativeHumidity) -> uint8_t
{
    const auto dv =
        216.7F * ((static_cast<float>(relativeHumidity) / 100.F) * 6.112F *
                  static_cast<float>(exp(static_cast<double>((17.62F * temperature) / (243.12F + temperature)))) /
                  (273.15F + temperature));
    return static_cast<uint8_t>(1000.0F * dv);
}

auto SensorManager::begin() noexcept -> bool
{
    if (_sgp.begin())
    {
        _dht.begin();
        _stationUpTime = millis();
        return true;
    }
    return false;
}

auto SensorManager::setInterval(uint32_t millis) noexcept -> bool
{
    if (millis >= minimalIntervalMs)
    {
        _interval = millis;
        return true;
    }
    return false;
}

auto SensorManager::areNewMeasurementsReady() const noexcept -> bool
{
    const auto currentTime = millis();
    return currentTime - _stationUpTime > startupDelayMs && currentTime - _lastMeasurementTime > _interval;
}

auto SensorManager::getLastMeasurement() const noexcept -> const Measurement&
{
    return _lastMeasurement;
}

auto SensorManager::performMeasurement() noexcept -> bool
{
    const auto humidity = _dht.readIntegerHumidity();
    const auto temperature = _dht.readTemperature();
    if (!humidity.isOk() || !temperature.isOk())
    {
        return false;
    }
    _sgp.setHumidity(getHumidity(temperature.value, static_cast<uint8_t>(humidity.value)));

    auto co2 = uint16_t {};
    auto voc = uint16_t {};

    if (_sgp.measureAirQuality(voc, co2))
    {
        _lastMeasurement = {voc, co2, temperature.value, static_cast<uint8_t>(humidity.value)};
        _lastMeasurementTime = millis();
        return true;
    }
    return false;
}

auto SensorManager::performMeasurementIfReady() noexcept -> bool
{
    if (areNewMeasurementsReady())
    {
        return performMeasurement();
    }
    return false;
}

auto SensorManager::getVoc() const noexcept -> uint16_t
{
    return _lastMeasurement.voc;
}

auto SensorManager::getCo2() const noexcept -> uint16_t
{
    return _lastMeasurement.co2;
}

auto SensorManager::getTemperature() const noexcept -> float
{
    return _lastMeasurement.temperature;
}

auto SensorManager::getHumidity() const noexcept -> uint8_t
{
    return _lastMeasurement.humidity;
}
