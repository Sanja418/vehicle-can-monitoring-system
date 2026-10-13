#include "speed_message.hpp"

#include <array>
#include <iostream>

int main()
{
    std::array<std::uint16_t, 6> values{0, 60, 130, 255, 256, 65535};

    for (std::uint16_t speed : values)
    {
        CanFrame frame = createSpeedFrame(speed);

        if (!isSpeedFrame(frame) || readSpeed(frame) != speed)
        {
            std::cerr << "Speed round-trip failed for: " << speed << '\n';
            return 1;
        }
    }

    CanFrame wrongId = createSpeedFrame(60);
    wrongId.id = 0x200;

    if (isSpeedFrame(wrongId))
    {
        std::cerr << "Wrong ID was accepted\n";
        return 1;
    }

    CanFrame wrongLength = createSpeedFrame(60);
    wrongLength.length = 1;

    if (isSpeedFrame(wrongLength))
    {
        std::cerr << "Wrong length was accepted\n";
        return 1;
    }

    std::cout << "Speed tests passed\n";
    return 0;
}
