#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "Array.hpp"
#include "ConstSpan.hpp"
#include "I2cDevice.hpp"
#include "Span.hpp"

class Sgp30
{
private:
    struct Command
    {
        uint16_t value;
        uint16_t delay;
        uint8_t requestLength;
        uint8_t responseLength;
    };

    static constexpr auto supportedFeatureSetVersion = uint16_t {0x0020};
    static constexpr auto successfulTest = uint16_t {0xD400};
    static constexpr auto getSerialIdCommand = Command {0x3682, 10, 0, 3};
    static constexpr auto getFeatureSetVersionCommand = Command {0x202F, 10, 0, 1};
    static constexpr auto initAirQualityCommand = Command {0x2003, 10, 0, 0};
    static constexpr auto measureAirQualityCommand = Command {0x2008, 12, 0, 2};
    static constexpr auto getBaselineCommand = Command {0x2015, 10, 0, 2};
    static constexpr auto setBaselineCommand = Command {0x201E, 10, 2, 0};
    static constexpr auto setHumidityCommand = Command {0x2061, 10, 1, 0};
    static constexpr auto measureTestCommand = Command {0x2032, 220, 0, 1};
    static constexpr auto measureRawSignalsCommand = Command {0x2050, 25, 0, 2};

public:
    auto begin() noexcept -> bool;

    auto measureAirQuality(uint16_t& voc, uint16_t& co2) const noexcept -> bool;
    auto measureRawSignals(uint16_t& ethanol, uint16_t& h2) const noexcept -> bool;
    auto getBaseline(uint16_t& voc, uint16_t& co2) const noexcept -> bool;
    auto setBaseline(uint16_t voc, uint16_t co2) const noexcept -> bool;
    auto setHumidity(uint8_t humidity) const noexcept -> bool;
    auto getSerialNumber(Array<uint16_t, getSerialIdCommand.responseLength>& serialNumber) const noexcept -> bool;
    auto measureTest() const noexcept -> bool;

private:
    template <uint32_t N>
    static auto prepareCommandBuffer(const Array<uint16_t, N>& command) noexcept
        -> Array<uint8_t, 2 + ((N - 1) * (sizeof(uint16_t) + 1))>
    {
        Array<uint8_t, 2 + ((N - 1) * (sizeof(uint16_t) + 1))> result;
        result[0] = static_cast<uint8_t>(command[0] >> 8U);
        result[1] = static_cast<uint8_t>(command[0] & 0xFFU);
        for (auto i = uint32_t {2}; i < result.size(); i += 3)
        {
            result[i] = static_cast<uint8_t>(command[i] >> 8U);
            result[i + 1] = static_cast<uint8_t>(command[i] & 0xFFU);
            result[i + 2] = generateCRC(ConstSpan<uint8_t> {result}.subspan(i, 2));
        }
        return result;
    }

    static auto generateCRC(ConstSpan<uint8_t> buffer) noexcept -> uint8_t;

    template <uint32_t N>
    auto pushCommand(const Array<uint16_t, N>& command) const noexcept -> bool
    {
        const auto commandBuffer = prepareCommandBuffer(command);
        return _device.write(ConstSpan<uint8_t> {commandBuffer});
    }

    template <uint32_t N>
    auto performCommand(const Array<uint16_t, N>& command, uint32_t delayMs) const noexcept -> bool
    {
        if (!pushCommand(command))
        {
            return false;
        }
        delay(delayMs);

        return true;
    }

    template <uint32_t N1, uint32_t N2>
    auto performCommand(const Array<uint16_t, N1>& command,
                        uint32_t delayMs,
                        Array<uint16_t, N2>& result) const noexcept -> bool
    {
        if (!pushCommand(command))
        {
            return false;
        }
        delay(delayMs);

        return readCommandResult(result);
    }

    template <uint32_t N>
    auto readCommandResult(Array<uint16_t, N>& result) const noexcept -> bool
    {
        static constexpr auto replyLength = N * (sizeof(uint16_t) + 1);
        Array<uint8_t, replyLength> buffer;

        if (!_device.read(Span<uint8_t>(buffer)))
        {
            return false;
        }

        for (auto i = uint32_t {}; i < N; i++)
        {
            if (generateCRC(ConstSpan<uint8_t> {buffer}.subspan(i * 3, 2)) != buffer[i * 3 + 2])
            {
                return false;
            }
        }

        for (auto i = uint32_t {}; i < N; i++)
        {
            result[i] = buffer[i * 3];
            result[i] <<= 8;
            result[i] |= buffer[(i * 3) + 1];
        }

        return true;
    }

    static constexpr auto address = uint8_t {0x58};
    Array<uint16_t, getSerialIdCommand.responseLength> _serialNumber {};
    I2cDevice _device {address};
};