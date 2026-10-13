#include "rpm_message.hpp"
#include "speed_message.hpp"
#include "temperature_message.hpp"
volatile unsigned int counter = 0;

volatile unsigned int& registerAt(unsigned int address)
{
    return *reinterpret_cast<volatile unsigned int*>(address);
}

void initUart()
{
    // Enable the UART4 peripheral clock.
    registerAt(0x40023840) |= (1u << 19);

    // Configure UART4 for transmission.
    registerAt(0x40004C0C) = 0;
    registerAt(0x40004C08) = 139;
    registerAt(0x40004C0C) = (1u << 13) | (1u << 3);
}

void writeUart(const char* text)
{
    while (*text != '\0')
    {
        // Wait until the transmit data register is empty.
        while ((registerAt(0x40004C00) & (1u << 7)) == 0)
        {
        }

        registerAt(0x40004C04) = static_cast<unsigned char>(*text);

        ++text;
    }
}
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
    // Enter filter configuration mode.
    registerAt(0x40006600) = (14u << 8) | 1u;

    // Disable filters while configuring them.
    registerAt(0x4000661C) = 0u;

    // Filter 0: mask mode, 32-bit scale, receive FIFO 0.
    registerAt(0x40006604) = 0u;
    registerAt(0x4000660C) = 1u;
    registerAt(0x40006614) = 0u;

    // Zero mask accepts every identifier.
    registerAt(0x40006640) = 0u;
    registerAt(0x40006644) = 0u;

    // Enable filter 0 and leave configuration mode.
    registerAt(0x4000661C) = 1u;
    registerAt(0x40006600) = 14u << 8;
}
bool initCan()
{
    // Enable the CAN1 peripheral clock.
    registerAt(0x40023840) = registerAt(0x40023840) | (1u << 25);

    // Clear sleep mode and request initialization.
    registerAt(0x40006400) = 1u;

    if (!waitCanInit(true))
    {
        return false;
    }

    // Bit timing: prescaler 2, segment 1 = 13, segment 2 = 2.
    registerAt(0x4000641C) = 1u | (12u << 16) | (1u << 20);

    // Leave initialization mode.
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
void writeNumber(unsigned short value)
{
    char text[6];
    unsigned int position = 5;
    text[position] = '\0';

    do
    {
        text[--position] = static_cast<char>('0' + value % 10);

        value = static_cast<unsigned short>(value / 10);
    } while (value != 0);

    writeUart(&text[position]);
}
int main()
{
    initUart();

    if (!initCan())
    {
        writeUart("CAN1 initialization failed!\r\n");

        while (true)
        {
        }
    }

#ifdef CAN_RECEIVER
    writeUart("CAN receiver ready!\r\n");

    while (true)
    {
        CanFrame frame{};

        if (receiveCanFrame(frame))
        {
            if (isRpmFrame(frame))
            {
                writeUart("Received RPM: ");
                writeNumber(readRpm(frame));
                writeUart("\r\n");
            }
            else if (isTemperatureFrame(frame))
            {
                std::int16_t temperature = readTemperature(frame);

                writeUart("Received temperature: ");

                if (temperature < 0)
                {
                    writeUart("-");
                }

                int magnitude = temperature;

                if (magnitude < 0)
                {
                    magnitude = -magnitude;
                }

                writeNumber(static_cast<unsigned short>(magnitude));
                writeUart(" C\r\n");
            }
            else if (isSpeedFrame(frame))
            {
                writeUart("Received speed: ");
                writeNumber(readSpeed(frame));
                writeUart(" km/h\r\n");
            }
        }
    }
#else
    writeUart("CAN sender ready!\r\n");

    if (sendRpm(1500))
    {
        writeUart("RPM transmission requested!\r\n");
    }
    else
    {
        writeUart("CAN transmit mailbox unavailable!\r\n");
    }
    if (sendCanFrame(createTemperatureFrame(-10)))
    {
        writeUart("Temperature transmission requested!\r\n");
    }
    else
    {
        writeUart("Temperature transmission request failed!\r\n");
    }
    if (sendCanFrame(createSpeedFrame(60)))
    {
        writeUart("Speed transmission requested!\r\n");
    }
    else
    {
        writeUart("Speed transmission request failed!\r\n");
    }
    while (true)
    {
        counter = counter + 1;
    }
#endif
}
