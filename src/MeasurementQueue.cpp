#include "MeasurementQueue.hpp"

#include <math.h>
#include <stdint.h>

#include "SensorManager.hpp"

auto MeasurementQueue::getNextMeasurement() noexcept -> uint8_t
{
    const auto measurement = getMeasurement();
    const auto result = addTypeToValue(measurement);

    ++_requestId;
    return result;
}

auto MeasurementQueue::castCo2To5bits(uint16_t co2) noexcept -> uint8_t
{
    if (co2 > 40000)
    {
        return 5;
    }
    if (co2 > 5000)
    {
        return 4;
    }
    if (co2 > 2000)
    {
        return 3;
    }
    if (co2 > 1000)
    {
        return 2;
    }
    if (co2 > 400)
    {
        return 1;
    }
    return 0;
}

auto MeasurementQueue::castVocTo5bits(uint16_t voc) noexcept -> uint8_t
{
    if (voc > 30000)
    {
        return 3;
    }
    if (voc > 2200)
    {
        return 2;
    }
    if (voc > 400)
    {
        return 1;
    }
    return 0;
}

auto MeasurementQueue::castTemperatureTo5Bits(float temperature) noexcept -> uint8_t
{
    if (temperature < 16.F)
    {
        return 0;
    }
    return static_cast<uint8_t>(floor(static_cast<double>((temperature - 16.F) * 2.F))) & maxValue;
}

auto MeasurementQueue::castHumidityTo5Bits(uint8_t humidity) noexcept -> uint8_t
{
    if (humidity < 20)
    {
        return 0;
    }
    return static_cast<uint8_t>((humidity - 20) / 2) & maxValue;
}

auto MeasurementQueue::addTypeToValue(uint8_t value) const noexcept -> uint8_t
{
    return static_cast<uint8_t>((_requestId % toInt(Type::MaxType)) << 5U) | value;
}

auto MeasurementQueue::getMeasurement() noexcept -> uint8_t
{
    const auto& measurement = _station.getLastMeasurement();
    switch (_requestId % toInt(Type::MaxType))
    {
    case toInt(Type::Co2):
        return castCo2To5bits(measurement.co2);
    case toInt(Type::Voc):
        return castVocTo5bits(measurement.voc);
    case toInt(Type::Temperature):
        return castTemperatureTo5Bits(measurement.temperature);
    case toInt(Type::Humidity):
        return castHumidityTo5Bits(measurement.humidity);
    default:
        return 0;
    }
}

MeasurementQueue::MeasurementQueue(const SensorManager& station) noexcept
    : _station {station}
{
}
