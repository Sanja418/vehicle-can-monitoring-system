#include "uart.hpp"

#include "register_access.hpp"
#include "stm32_registers.hpp"

void initUart()
{
    registerAt(stm32::RCC_APB1ENR) = registerAt(stm32::RCC_APB1ENR) | (1u << 19);

    registerAt(stm32::UART4_CR1) = 0u;
    registerAt(stm32::UART4_BRR) = 139u;
    registerAt(stm32::UART4_CR1) = (1u << 13) | (1u << 3);
}

void writeUart(const char* text)
{
    while (*text != '\0')
    {
        while ((registerAt(stm32::UART4_SR) & (1u << 7)) == 0)
        {
        }

        registerAt(stm32::UART4_DR) = static_cast<unsigned char>(*text);

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
