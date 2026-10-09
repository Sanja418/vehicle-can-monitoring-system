#include <array>
#include <chrono>
#include <iostream>
#include <thread>

#include "can_bus.hpp"
#include "dashboard_node.hpp"
#include "engine_node.hpp"

int main()
{
    EngineNode engine{0};
    DashboardNode dashboard;
    CanBus bus;
    CanFrame emptyFrame{};

    if (!bus.receive(emptyFrame))
    {
        std::cout << "CAN queue is empty\n";
    }
    dashboard.printStatus(std::chrono::seconds(2));
    std::cout << "Has RPM before: " << dashboard.hasRpm() << '\n';

    std::array<std::uint16_t, 6> rpmValues{800, 1500, 2500, 3200, 900, 0};

    for (std::uint16_t rpm : rpmValues)
    {
        engine.setRpm(rpm);

        bus.send(engine.createFrame());

        CanFrame receivedFrame{};

        if (bus.receive(receivedFrame))
        {
            dashboard.receiveFrame(receivedFrame);
        }
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
    std::cout << "Communication restored\n";

    engine.setRpm(1200);

    bus.send(engine.createFrame());

    CanFrame recoveryFrame{};

    if (bus.receive(recoveryFrame))
    {
        dashboard.receiveFrame(recoveryFrame);
    }

    dashboard.printStatus(std::chrono::seconds(2));
    CanFrame shortFrame = engine.createFrame();
    shortFrame.length = 1;

    dashboard.receiveFrame(shortFrame);

    std::cout << "RPM after short frame: " << dashboard.getRpm() << '\n';
    std::cout << "Queued RPM messages\n";

    engine.setRpm(1000);
    bus.send(engine.createFrame());

    engine.setRpm(2000);
    bus.send(engine.createFrame());

    engine.setRpm(3000);
    bus.send(engine.createFrame());

    CanFrame queuedFrame{};

    while (bus.receive(queuedFrame))
    {
        dashboard.receiveFrame(queuedFrame);
        dashboard.printStatus(std::chrono::seconds(2));
    }
    return 0;
}
