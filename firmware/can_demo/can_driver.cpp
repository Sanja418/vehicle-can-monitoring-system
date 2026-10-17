#include "can_driver.hpp"

#include "register_access.hpp"
#include "rpm_message.hpp"

bool waitCanInit(bool expected)
{
    for (unsigned int attempt = 0; attempt < 100000; ++attempt)
    {
        bool acknowledged = (registerAt(0x40006404) & 1u) != 0;

        if (acknowledged == expected)
        {
            return true;
        }
    }

    return false;
}

void configureCanFilter()
{
    registerAt(0x40006600) = (14u << 8) | 1u;
    registerAt(0x4000661C) = 0u;

    registerAt(0x40006604) = 0u;
    registerAt(0x4000660C) = 1u;
    registerAt(0x40006614) = 0u;

    registerAt(0x40006640) = 0u;
    registerAt(0x40006644) = 0u;

    registerAt(0x4000661C) = 1u;
    registerAt(0x40006600) = 14u << 8;
}

bool initCan()
{
    registerAt(0x40023840) = registerAt(0x40023840) | (1u << 25);

    registerAt(0x40006400) = 1u;

    if (!waitCanInit(true))
    {
        return false;
    }

    registerAt(0x4000641C) = 1u | (12u << 16) | (1u << 20);

    configureCanFilter();

    registerAt(0x40006400) = 0u;

    return waitCanInit(false);
}

bool sendCanFrame(const CanFrame& frame)
{
    if (frame.id > 0x7FFu || frame.length > 8)
    {
        return false;
    }

    unsigned int lowData = 0;
    unsigned int highData = 0;

    for (unsigned int i = 0; i < frame.length; ++i)
    {
        if (i < 4)
        {
            lowData |= static_cast<unsigned int>(frame.data[i]) << (8 * i);
        }
        else
        {
            highData |= static_cast<unsigned int>(frame.data[i]) << (8 * (i - 4));
        }
    }

    for (unsigned int attempt = 0; attempt < 100000; ++attempt)
    {
        if ((registerAt(0x40006408) & (1u << 26)) != 0)
        {
            unsigned int identifier = frame.id << 21;

            registerAt(0x40006580) = identifier;
            registerAt(0x40006584) = frame.length;
            registerAt(0x40006588) = lowData;
            registerAt(0x4000658C) = highData;

            registerAt(0x40006580) = identifier | 1u;

            return true;
        }
    }

    return false;
}

bool sendRpm(unsigned short rpm)
{
    return sendCanFrame(createRpmFrame(rpm));
}

bool receiveCanFrame(CanFrame& frame)
{
    if ((registerAt(0x4000640C) & 3u) == 0)
    {
        return false;
    }

    unsigned int identifier = registerAt(0x400065B0);
    unsigned int length = registerAt(0x400065B4) & 0xFu;
    unsigned int lowData = registerAt(0x400065B8);
    unsigned int highData = registerAt(0x400065BC);

    registerAt(0x4000640C) = 1u << 5;

    if ((identifier & 6u) != 0 || length > 8)
    {
        return false;
    }

    frame = CanFrame{};
    frame.id = (identifier >> 21) & 0x7FFu;
    frame.length = static_cast<std::uint8_t>(length);

    for (unsigned int i = 0; i < length; ++i)
    {
        if (i < 4)
        {
            frame.data[i] = static_cast<std::uint8_t>((lowData >> (8 * i)) & 0xFFu);
        }
        else
        {
            frame.data[i] = static_cast<std::uint8_t>((highData >> (8 * (i - 4))) & 0xFFu);
        }
    }

    return true;
}

bool receiveRpm(unsigned short& rpm)
{
    CanFrame frame{};

    if (!receiveCanFrame(frame) || !isRpmFrame(frame))
    {
        return false;
    }

    rpm = readRpm(frame);
    return true;
}
