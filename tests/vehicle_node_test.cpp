#include "vehicle_node.hpp"

#include <iostream>

#include "speed_message.hpp"

int main()
{
    VehicleNode vehicle{60};

    CanFrame initialFrame = vehicle.createFrame();

    if (!isSpeedFrame(initialFrame) || readSpeed(initialFrame) != 60)
    {
        std::cerr << "Initial vehicle speed is incorrect\n";
        return 1;
    }

    vehicle.setSpeed(120);

    CanFrame updatedFrame = vehicle.createFrame();

    if (!isSpeedFrame(updatedFrame) || readSpeed(updatedFrame) != 120)
    {
        std::cerr << "Updated vehicle speed is incorrect\n";
        return 1;
    }

    if (readSpeed(initialFrame) != 60)
    {
        std::cerr << "Speed update changed an earlier frame\n";
        return 1;
    }

    vehicle.setSpeed(0);

    CanFrame stoppedFrame = vehicle.createFrame();

    if (!isSpeedFrame(stoppedFrame) || readSpeed(stoppedFrame) != 0)
    {
        std::cerr << "Stopped vehicle speed is incorrect\n";
        return 1;
    }

    std::cout << "Vehicle tests passed\n";
    return 0;
}
