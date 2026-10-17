#include "timer.hpp"

#include "register_access.hpp"

void initTimer()
{
    registerAt(0x40023840) = registerAt(0x40023840) | 1u;

    registerAt(0x40000000) = 0u;

    // Renode platform: 10 MHz / 10000 = 1000 ticks per second.
    registerAt(0x40000028) = 9999u;
    registerAt(0x4000002C) = 0xFFFFFFFFu;

    registerAt(0x40000014) = 1u;
    registerAt(0x40000010) = 0u;
    registerAt(0x40000024) = 0u;

    registerAt(0x40000000) = 1u;
}

unsigned int readMilliseconds()
{
    return registerAt(0x40000024);
}
