#include <Arduino.h>
#include <HardwareSerial.h>
#include <Print.h>
#include <avr/pgmspace.h>
#include <hal/hal.h>
#include <lmic/lmic.h>
#include <lmic/lorabase.h>
#include <lmic/oslmic.h>
#include <lmic/oslmic_types.h>
#include <stdint.h>

#include "Array.hpp"
#include "MeasurementQueue.hpp"
#include "SensorManager.hpp"

namespace
{

constexpr uint8_t dhtPin = 7;
SensorManager station {dhtPin};
MeasurementQueue queue {station};

const Array<uint8_t, 8> PROGMEM appEui = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const Array<uint8_t, 8> PROGMEM devEui = {0x19, 0xB7, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
const Array<uint8_t, 16> PROGMEM appKey = {
    0x05, 0x08, 0xD2, 0x76, 0xB3, 0x5E, 0x24, 0x02, 0xB1, 0x77, 0x8E, 0x6A, 0xE1, 0x25, 0x74, 0x0F};

uint8_t payload;
osjob_t job;

constexpr auto interval = uint32_t {20};

auto sendJob(osjob_t* /*job*/) -> void
{
    if ((LMIC.opmode & static_cast<uint32_t>(OP_TXRXPEND)) != 0)
    {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        payload = queue.getNextMeasurement();
        LMIC_setTxData2(1, &payload, sizeof(payload), 0);
        Serial.print(F("Packet queued: "));
        Serial.println(payload, BIN);
    }
}

auto setupLora() -> void
{
    os_init();
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 10 / 100);
    LMIC_setLinkCheckMode(0);
    LMIC_setAdrMode(0);
    LMIC.dn2Dr = DR_SF9;
    LMIC_setDrTxpow(DR_SF9, 20);

    sendJob(&job);
}

}

auto os_getArtEui(u1_t* buffer) -> void
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
    memcpy_P(buffer, appEui.data, appEui.size());
}

auto os_getDevEui(u1_t* buffer) -> void
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
    memcpy_P(buffer, devEui.data, devEui.size());
}

auto os_getDevKey(u1_t* buffer) -> void
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
    memcpy_P(buffer, appKey.data, appKey.size());
}

const lmic_pinmap lmic_pins = {
    10, LMIC_UNUSED_PIN, 5, {2, 3, LMIC_UNUSED_PIN},
       0, 8, 8000000, nullptr
};

auto onEvent(ev_t event) -> void
{
    switch (event)
    {
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
        LMIC_setLinkCheckMode(0);
        break;
    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        if (!station.performMeasurementIfReady())
        {
            Serial.println("New measurements are not ready");
        }
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        os_setTimedCallback(&job, os_getTime() + sec2osticks(interval), sendJob);
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;
    case EV_REJOIN_FAILED:
    case EV_JOIN_FAILED:
    case EV_RFU1:
    case EV_JOINING:
    case EV_BEACON_TRACKED:
    case EV_BEACON_MISSED:
    case EV_BEACON_FOUND:
    case EV_SCAN_TIMEOUT:
    case EV_RXSTART:
    case EV_TXCANCELED:
    case EV_TXSTART:
    case EV_SCAN_FOUND:
    case EV_LINK_ALIVE:
    case EV_LINK_DEAD:
    case EV_RXCOMPLETE:
    case EV_RESET:
    case EV_LOST_TSYNC:
    default:
        break;
    }
}

auto setup() -> void
{
    Serial.begin(9600);

    if (station.begin())
    {
        Serial.println(F("SensorManager initialized correctly"));
    }

    setupLora();
}

auto loop() -> void
{
    os_runloop_once();
}