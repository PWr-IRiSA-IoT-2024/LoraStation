#include "Station.hpp"

auto Station::begin() noexcept -> bool
{
    if (_sgp.begin())
    {
        _dht.begin();
        _stationUpTime = millis();
        return true;
    }
    return false;
}

auto Station::setInterval(uint32_t millis) noexcept -> bool
{
    if (millis >= minimalIntervalMs)
    {
        _interval = millis;
        return true;
    }
    return false;
}

auto Station::areNewMeasurementsReady() noexcept -> bool
{
    const auto currentTime = millis();
    if (currentTime - _stationUpTime > startupDelayMs && currentTime - _lastMeasurementTime > _interval &&
        performMeasurement())
    {
        _lastMeasurementTime = millis();
        return true;
    }
    return false;
}

auto Station::getLastMeasurement() noexcept -> const Measurement&
{
    return _lastMeasurement;
}

auto Station::performMeasurement() noexcept -> bool
{
    const auto humidity = _dht.readIntegerHumidity();
    const auto temperature = _dht.readIntegerTemperature();
    auto co2 = uint16_t {};
    auto voc = uint16_t {};
    if (humidity.isOk() && temperature.isOk() && _sgp.measureAirQuality(voc, co2))
    {
        _lastMeasurement = {voc, co2, temperature.value, humidity.value};
        return true;
    }
    return false;
}
