#include <array>
#include <chrono>
#include <iostream>
#include <thread>

#include "can_bus.hpp"
#include "dashboard_node.hpp"
#include "engine_node.hpp"
#include "speed_message.hpp"
#include "temperature_message.hpp"
#include "vehicle_node.hpp"
void processMessages(CanBus& bus, DashboardNode& dashboard)
{
    CanFrame frame{};

    while (bus.receive(frame))
    {
        dashboard.receiveFrame(frame);
    }
}
void sendEngineMessages(const EngineNode& engine, CanBus& bus)
{
    bus.send(engine.createFrame());
    bus.send(engine.createTemperatureFrame());
}
void runEngineUpdate(EngineNode& engine, CanBus& bus, DashboardNode& dashboard)
{
    std::cout << "Sending both engine values...\n";

    engine.setRpm(1800);
    engine.setTemperature(85);

    sendEngineMessages(engine, bus);
    processMessages(bus, dashboard);

    dashboard.printStatus(std::chrono::seconds(2));
}
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

        processMessages(bus, dashboard);
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

    processMessages(bus, dashboard);

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
    CanFrame temperatureFrame = createTemperatureFrame(-10);

    if (isTemperatureFrame(temperatureFrame))
    {
        std::cout << "Temperature: " << readTemperature(temperatureFrame) << " C\n";
    }
    std::cout << "Has temperature before: " << dashboard.hasTemperature() << '\n';

    bus.send(createTemperatureFrame(-10));

    CanFrame receivedTemperatureFrame{};

    if (bus.receive(receivedTemperatureFrame))
    {
        dashboard.receiveFrame(receivedTemperatureFrame);
    }

    std::cout << "Has temperature after: " << dashboard.hasTemperature() << '\n';

    std::cout << "Dashboard temperature: " << dashboard.getTemperature() << " C\n";

    std::cout << "RPM after temperature: " << dashboard.getRpm() << '\n';
    dashboard.printStatus(std::chrono::seconds(2));
    std::cout << "Waiting without new messages...\n";

    std::this_thread::sleep_for(std::chrono::seconds(3));

    dashboard.printStatus(std::chrono::seconds(2));
    std::cout << "Sending only temperature...\n";

    engine.setTemperature(90);
    bus.send(engine.createTemperatureFrame());

    processMessages(bus, dashboard);

    dashboard.printStatus(std::chrono::seconds(2));
    runEngineUpdate(engine, bus, dashboard);
    std::cout << "Simulating engine overheating...\n";

    engine.setTemperature(115);
    bus.send(engine.createTemperatureFrame());

    processMessages(bus, dashboard);
    dashboard.printStatus(std::chrono::seconds(2));
    std::cout << "Sending vehicle speed...\n";

    VehicleNode vehicle{60};
    bus.send(vehicle.createFrame());
    processMessages(bus, dashboard);

    dashboard.printStatus(std::chrono::seconds(2));
    return 0;
}
