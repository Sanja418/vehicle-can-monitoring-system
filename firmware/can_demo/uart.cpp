#include "uart.hpp"

#include "register_access.hpp"

void initUart()
{
    registerAt(0x40023840) = registerAt(0x40023840) | (1u << 19);

    registerAt(0x40004C0C) = 0u;
    registerAt(0x40004C08) = 139u;
    registerAt(0x40004C0C) = (1u << 13) | (1u << 3);
}

void writeUart(const char* text)
{
    while (*text != '\0')
    {
        while ((registerAt(0x40004C00) & (1u << 7)) == 0)
        {
        }

        registerAt(0x40004C04) = static_cast<unsigned char>(*text);

        ++text;
    }
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
