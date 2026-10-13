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

int main()
{
    initUart();
    writeUart("Vehicle monitoring firmware started!\r\n");

    while (true)
    {
        counter = counter + 1;
    }
}
