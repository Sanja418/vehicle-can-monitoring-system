#include <array>
#include <chrono>
#include <iostream>
#include <thread>

#include "dashboard_node.hpp"
#include "engine_node.hpp"

int main()
{
    EngineNode engine{0};
    DashboardNode dashboard;
    dashboard.printStatus(std::chrono::seconds(2));
    std::cout << "Has RPM before: " << dashboard.hasRpm() << '\n';

    std::array<std::uint16_t, 6> rpmValues{800, 1500, 2500, 3200, 900, 0};

    for (std::uint16_t rpm : rpmValues)
    {
        engine.setRpm(rpm);

        CanFrame frame = engine.createFrame();
        dashboard.receiveFrame(frame);
        dashboard.printStatus(std::chrono::seconds(2));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "RPM fresh: " << dashboard.isRpmFresh(std::chrono::seconds(2)) << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(3));
    dashboard.printStatus(std::chrono::seconds(2));

    std::cout << "RPM fresh after waiting: " << dashboard.isRpmFresh(std::chrono::seconds(2))
              << '\n';
    std::cout << "Has RPM after: " << dashboard.hasRpm() << '\n';
    std::cout << "Last stored RPM: " << dashboard.getRpm() << '\n';
    CanFrame invalidFrame{};
    invalidFrame.id = 0x200;
    invalidFrame.length = 2;

    dashboard.receiveFrame(invalidFrame);

    std::cout << "RPM after invalid frame: " << dashboard.getRpm() << '\n';
    return 0;
}
