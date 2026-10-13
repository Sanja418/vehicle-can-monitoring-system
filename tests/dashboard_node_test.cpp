#include "dashboard_node.hpp"

#include <iostream>

#include "rpm_message.hpp"
#include "speed_message.hpp"
#include "temperature_message.hpp"

int main()
{
    DashboardNode dashboard;

    if (dashboard.hasRpm() || dashboard.hasTemperature())
    {
        std::cerr << "New dashboard should have no data\n";
        return 1;
    }

    dashboard.receiveFrame(createRpmFrame(0));

    if (!dashboard.hasRpm() || dashboard.getRpm() != 0)
    {
        std::cerr << "Zero RPM was not stored correctly\n";
        return 1;
    }

    dashboard.receiveFrame(createRpmFrame(2500));

    CanFrame invalid = createRpmFrame(9999);
    invalid.length = 1;
    dashboard.receiveFrame(invalid);

    if (dashboard.getRpm() != 2500)
    {
        std::cerr << "Invalid frame changed stored RPM\n";
        return 1;
    }

    dashboard.receiveFrame(createTemperatureFrame(-10));

    if (!dashboard.hasTemperature() || dashboard.getTemperature() != -10 ||
        dashboard.getRpm() != 2500)
    {
        std::cerr << "Temperature or independent RPM state is wrong\n";
        return 1;
    }
    DashboardNode timedDashboard;

    auto start = std::chrono::steady_clock::time_point{};
    auto timeout = std::chrono::seconds(2);

    if (timedDashboard.isRpmFresh(timeout, start) ||
        timedDashboard.isTemperatureFresh(timeout, start))
    {
        std::cerr << "Missing data should not be fresh\n";
        return 1;
    }

    timedDashboard.receiveFrame(createRpmFrame(1500), start);
    timedDashboard.receiveFrame(createTemperatureFrame(90), start);

    auto beforeDeadline = start + std::chrono::milliseconds(1999);

    if (!timedDashboard.isRpmFresh(timeout, beforeDeadline) ||
        !timedDashboard.isTemperatureFresh(timeout, beforeDeadline))
    {
        std::cerr << "Data became stale too early\n";
        return 1;
    }

    auto deadline = start + timeout;

    if (timedDashboard.isRpmFresh(timeout, deadline) ||
        timedDashboard.isTemperatureFresh(timeout, deadline))
    {
        std::cerr << "Data should be stale at the deadline\n";
        return 1;
    }

    auto later = start + std::chrono::seconds(3);
    CanFrame invalidTimedFrame = createRpmFrame(9999);
    invalidTimedFrame.length = 1;

    timedDashboard.receiveFrame(invalidTimedFrame, later);

    if (timedDashboard.isRpmFresh(timeout, later) || timedDashboard.getRpm() != 1500)
    {
        std::cerr << "Invalid frame refreshed or changed RPM\n";
        return 1;
    }

    timedDashboard.receiveFrame(createTemperatureFrame(85), later);

    if (timedDashboard.isRpmFresh(timeout, later) ||
        !timedDashboard.isTemperatureFresh(timeout, later))
    {
        std::cerr << "Temperature update incorrectly refreshed RPM\n";
        return 1;
    }

    timedDashboard.receiveFrame(createRpmFrame(1800), later);

    if (!timedDashboard.isRpmFresh(timeout, later))
    {
        std::cerr << "RPM did not recover after a new message\n";
        return 1;
    }
    DashboardNode warningDashboard;

    auto warningTime = std::chrono::steady_clock::time_point{};
    auto warningTimeout = std::chrono::seconds(2);

    if (warningDashboard.isEngineOverheated(warningTimeout, warningTime))
    {
        std::cerr << "Missing temperature triggered a warning\n";
        return 1;
    }

    warningDashboard.receiveFrame(createTemperatureFrame(109), warningTime);

    if (warningDashboard.isEngineOverheated(warningTimeout, warningTime))
    {
        std::cerr << "Temperature below threshold triggered a warning\n";
        return 1;
    }

    warningDashboard.receiveFrame(createTemperatureFrame(110), warningTime);

    if (!warningDashboard.isEngineOverheated(warningTimeout, warningTime))
    {
        std::cerr << "Temperature at threshold did not trigger a warning\n";
        return 1;
    }

    warningDashboard.receiveFrame(createTemperatureFrame(120), warningTime);

    if (!warningDashboard.isEngineOverheated(warningTimeout, warningTime))
    {
        std::cerr << "Temperature above threshold did not trigger a warning\n";
        return 1;
    }

    auto staleWarningTime = warningTime + warningTimeout;

    if (warningDashboard.isEngineOverheated(warningTimeout, staleWarningTime))
    {
        std::cerr << "Stale temperature triggered a current warning\n";
        return 1;
    }
    DashboardNode speedDashboard;

    auto speedStart = std::chrono::steady_clock::time_point{};
    auto speedTimeout = std::chrono::seconds(2);

    if (speedDashboard.hasSpeed() || speedDashboard.isSpeedFresh(speedTimeout, speedStart))
    {
        std::cerr << "New dashboard should have no speed data\n";
        return 1;
    }

    speedDashboard.receiveFrame(createSpeedFrame(0), speedStart);

    if (!speedDashboard.hasSpeed() || speedDashboard.getSpeed() != 0)
    {
        std::cerr << "Zero speed was not stored correctly\n";
        return 1;
    }

    speedDashboard.receiveFrame(createSpeedFrame(60), speedStart);

    auto speedBeforeDeadline = speedStart + std::chrono::milliseconds(1999);
    auto speedDeadline = speedStart + speedTimeout;

    if (!speedDashboard.isSpeedFresh(speedTimeout, speedBeforeDeadline) ||
        speedDashboard.isSpeedFresh(speedTimeout, speedDeadline))
    {
        std::cerr << "Speed freshness deadline is incorrect\n";
        return 1;
    }

    CanFrame invalidSpeed = createSpeedFrame(999);
    invalidSpeed.length = 1;
    speedDashboard.receiveFrame(invalidSpeed, speedDeadline);

    if (speedDashboard.getSpeed() != 60 || speedDashboard.isSpeedFresh(speedTimeout, speedDeadline))
    {
        std::cerr << "Invalid frame changed or refreshed speed\n";
        return 1;
    }

    speedDashboard.receiveFrame(createRpmFrame(2000), speedDeadline);
    speedDashboard.receiveFrame(createTemperatureFrame(90), speedDeadline);

    if (speedDashboard.isSpeedFresh(speedTimeout, speedDeadline))
    {
        std::cerr << "Other messages refreshed speed\n";
        return 1;
    }

    speedDashboard.receiveFrame(createSpeedFrame(80), speedDeadline);

    if (speedDashboard.getSpeed() != 80 ||
        !speedDashboard.isSpeedFresh(speedTimeout, speedDeadline) ||
        speedDashboard.getRpm() != 2000 || speedDashboard.getTemperature() != 90)
    {
        std::cerr << "Speed recovery or independent data is incorrect\n";
        return 1;
    }
    std::cout << "Dashboard state tests passed\n";
    return 0;
}
