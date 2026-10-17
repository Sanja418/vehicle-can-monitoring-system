#include "can_driver.hpp"

#include "register_access.hpp"
#include "rpm_message.hpp"
#include "stm32_registers.hpp"

bool waitCanInit(bool expected)
{
    for (unsigned int attempt = 0; attempt < 100000; ++attempt)
    {
        bool acknowledged = (registerAt(stm32::CAN1_MSR) & 1u) != 0;

        if (acknowledged == expected)
        {
            return true;
        }
    }

    return false;
}

void configureCanFilter()
{
    registerAt(stm32::CAN1_FMR) = (14u << 8) | 1u;
    registerAt(stm32::CAN1_FA1R) = 0u;

    registerAt(stm32::CAN1_FM1R) = 0u;
    registerAt(stm32::CAN1_FS1R) = 1u;
    registerAt(stm32::CAN1_FFA1R) = 0u;

    registerAt(stm32::CAN1_F0R1) = 0u;
    registerAt(stm32::CAN1_F0R2) = 0u;

    registerAt(stm32::CAN1_FA1R) = 1u;
    registerAt(stm32::CAN1_FMR) = 14u << 8;
}

bool initCan()
{
    registerAt(stm32::RCC_APB1ENR) = registerAt(stm32::RCC_APB1ENR) | (1u << 25);

    registerAt(stm32::CAN1_MCR) = 1u;

    if (!waitCanInit(true))
    {
        return false;
    }

    registerAt(stm32::CAN1_BTR) = 1u | (12u << 16) | (1u << 20);

    configureCanFilter();

    registerAt(stm32::CAN1_MCR) = 0u;

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
        if ((registerAt(stm32::CAN1_TSR) & (1u << 26)) != 0)
        {
            unsigned int identifier = frame.id << 21;

            registerAt(stm32::CAN1_TI0R) = identifier;
            registerAt(stm32::CAN1_TDT0R) = frame.length;
            registerAt(stm32::CAN1_TDL0R) = lowData;
            registerAt(stm32::CAN1_TDH0R) = highData;

            registerAt(stm32::CAN1_TI0R) = identifier | 1u;

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
    if ((registerAt(stm32::CAN1_RF0R) & 3u) == 0)
    {
        return false;
    }

    unsigned int identifier = registerAt(stm32::CAN1_RI0R);
    unsigned int length = registerAt(stm32::CAN1_RDT0R) & 0xFu;
    unsigned int lowData = registerAt(stm32::CAN1_RDL0R);
    unsigned int highData = registerAt(stm32::CAN1_RDH0R);

    registerAt(stm32::CAN1_RF0R) = 1u << 5;

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
