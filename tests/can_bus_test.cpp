#include "can_bus.hpp"

#include <array>
#include <iostream>

#include "rpm_message.hpp"

int main()
{
    CanBus bus;
    CanFrame received{};

    if (bus.receive(received))
    {
        std::cerr << "Empty queue returned a message\n";
        return 1;
    }

    std::array<std::uint16_t, 3> values{1000, 2000, 3000};
    CanFrame sent{};

    for (std::uint16_t rpm : values)
    {
        sent = createRpmFrame(rpm);
        bus.send(sent);
    }

    // Changing the original must not change queued copies.
    sent = createRpmFrame(9999);

    for (std::uint16_t expected : values)
    {
        if (!bus.receive(received))
        {
            std::cerr << "Expected message is missing\n";
            return 1;
        }

        if (!isRpmFrame(received) || readRpm(received) != expected)
        {
            std::cerr << "Wrong message or queue order\n";
            return 1;
        }
    }

    if (bus.receive(received))
    {
        std::cerr << "Queue should be empty after receiving\n";
        return 1;
    }

    std::cout << "CAN queue tests passed\n";
    return 0;
}
