#include "Sgp30.hpp"

#include <stdint.h>

#include "Array.hpp"
#include "ConstSpan.hpp"

constexpr Sgp30::Command Sgp30::getSerialIdCommand;           // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::getFeatureSetVersionCommand;  // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::initAirQualityCommand;        // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::measureAirQualityCommand;     // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::getBaselineCommand;           // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::setBaselineCommand;           // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::setHumidityCommand;           // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::measureTestCommand;           // NOLINT(readability-redundant-declaration)
constexpr Sgp30::Command Sgp30::measureRawSignalsCommand;     // NOLINT(readability-redundant-declaration)

auto Sgp30::begin() noexcept -> bool
{
    if (!_device.begin())
    {
        return false;
    }

    if (!performCommand(Array<uint16_t, 1> {getSerialIdCommand.value}, getSerialIdCommand.delay, _serialNumber))
    {
        return false;
    }

    Array<uint16_t, getFeatureSetVersionCommand.responseLength> featureSetVersion {};
    if (!performCommand(Array<uint16_t, 1> {getFeatureSetVersionCommand.value},
                        getFeatureSetVersionCommand.delay,
                        featureSetVersion))
    {
        return false;
    }

    if ((featureSetVersion[0] & 0xF0U) != supportedFeatureSetVersion)
    {
        return false;
    }

    return performCommand(Array<uint16_t, 1> {initAirQualityCommand.value}, initAirQualityCommand.delay);
}

auto Sgp30::measureAirQuality(uint16_t& voc, uint16_t& co2) const noexcept -> bool
{
    Array<uint16_t, measureAirQualityCommand.responseLength> response {};
    if (!performCommand(Array<uint16_t, 1> {measureAirQualityCommand.value}, measureAirQualityCommand.delay, response))
    {
        return false;
    }

    Serial.println(co2);

    voc = response[1];
    co2 = response[0];

    return true;
}

auto Sgp30::generateCRC(ConstSpan<uint8_t> buffer) noexcept -> uint8_t
{
    auto result = uint8_t {0xFFU};
    for (const auto byte : buffer)
    {
        result ^= byte;
        for (uint32_t i = 0; i < 8; i++)
        {
            if ((result & 0x80U) != 0)
            {
                static constexpr auto crcPolynomial = uint8_t {0x31};
                result = static_cast<uint8_t>(static_cast<uint8_t>(result << 1U) ^ crcPolynomial);
            }
            else
            {
                result = static_cast<uint8_t>(result << 1U);
            }
        }
    }
    return result;
}

auto Sgp30::getBaseline(uint16_t& voc, uint16_t& co2) const noexcept -> bool
{
    Array<uint16_t, getBaselineCommand.responseLength> response {};
    if (!performCommand(Array<uint16_t, 1> {getBaselineCommand.value}, getBaselineCommand.delay, response))
    {
        return false;
    }

    voc = response[1];
    co2 = response[0];

    return true;
}

auto Sgp30::setBaseline(uint16_t voc, uint16_t co2) const noexcept -> bool
{
    const Array<uint16_t, setBaselineCommand.requestLength + 1> request {setBaselineCommand.value, voc, co2};
    return performCommand(request, setBaselineCommand.delay);
}

auto Sgp30::setHumidity(uint8_t humidity) const noexcept -> bool
{
    const Array<uint16_t, setHumidityCommand.requestLength + 1> request {setHumidityCommand.value,
                                                                         static_cast<uint16_t>(humidity << 8U)};
    return performCommand(request, setHumidityCommand.delay);
}

auto Sgp30::measureRawSignals(uint16_t& ethanol, uint16_t& h2) const noexcept -> bool
{
    Array<uint16_t, measureRawSignalsCommand.responseLength> response {};
    if (!performCommand(Array<uint16_t, 1> {measureRawSignalsCommand.value}, measureRawSignalsCommand.delay, response))
    {
        return false;
    }

    ethanol = response[1];
    h2 = response[0];

    return true;
}

auto Sgp30::getSerialNumber(Array<uint16_t, getSerialIdCommand.responseLength>& serialNumber) const noexcept -> bool
{
    serialNumber = _serialNumber;
    return true;
}

auto Sgp30::measureTest() const noexcept -> bool
{
    Array<uint16_t, measureTestCommand.responseLength> test {};
    if (!performCommand(Array<uint16_t, 1> {measureTestCommand.value}, measureTestCommand.delay, test))
    {
        return false;
    }

    return test[0] == supportedFeatureSetVersion;
}
