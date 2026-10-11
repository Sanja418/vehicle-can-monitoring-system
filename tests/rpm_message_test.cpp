#include "rpm_message.hpp"

#include <array>
#include <iostream>

int main()
{
    std::array<std::uint16_t, 5> values{0, 255, 256, 2500, 65535};

    for (std::uint16_t rpm : values)
    {
        CanFrame frame = createRpmFrame(rpm);

        if (!isRpmFrame(frame) || readRpm(frame) != rpm)
        {
            std::cerr << "RPM round-trip failed for: " << rpm << '\n';
            return 1;
        }
    }

    CanFrame wrongId = createRpmFrame(1500);
    wrongId.id = 0x200;

    if (isRpmFrame(wrongId))
    {
        std::cerr << "Wrong ID was accepted\n";
        return 1;
    }

    CanFrame wrongLength = createRpmFrame(1500);
    wrongLength.length = 1;

    if (isRpmFrame(wrongLength))
    {
        std::cerr << "Wrong length was accepted\n";
        return 1;
    }

    std::cout << "RPM tests passed\n";
    return 0;
}
