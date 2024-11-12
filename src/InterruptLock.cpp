#include "InterruptLock.hpp"

#include <Arduino.h>

InterruptLock::InterruptLock() noexcept
{
    noInterrupts();
}

InterruptLock::~InterruptLock() noexcept
{
    interrupts();
}