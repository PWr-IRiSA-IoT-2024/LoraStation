#pragma once

class InterruptLock
{
public:
    InterruptLock() noexcept;
    InterruptLock(const InterruptLock& lock) = delete;
    InterruptLock(InterruptLock&& lock) = delete;
    auto operator=(const InterruptLock& lock) -> InterruptLock& = delete;
    auto operator=(InterruptLock&& lock) -> InterruptLock& = delete;
    ~InterruptLock() noexcept;
};
