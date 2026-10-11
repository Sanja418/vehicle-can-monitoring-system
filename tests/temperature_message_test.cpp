#include "temperature_message.hpp"

#include <array>
#include <iostream>

int main()
{
    std::array<std::int16_t, 7> values{-32768, -10, -1, 0, 90, 256, 32767};

    for (std::int16_t temperature : values)
    {
        CanFrame frame = createTemperatureFrame(temperature);

        if (!isTemperatureFrame(frame) || readTemperature(frame) != temperature)
        {
            std::cerr << "Temperature round-trip failed for: " << temperature << '\n';
            return 1;
        }
    }

    CanFrame wrongId = createTemperatureFrame(90);
    wrongId.id = 0x200;

    if (isTemperatureFrame(wrongId))
    {
        std::cerr << "Wrong ID was accepted\n";
        return 1;
    }

    CanFrame wrongLength = createTemperatureFrame(90);
    wrongLength.length = 1;

    if (isTemperatureFrame(wrongLength))
    {
        std::cerr << "Wrong length was accepted\n";
        return 1;
    }

    std::cout << "Temperature tests passed\n";
    return 0;
}
