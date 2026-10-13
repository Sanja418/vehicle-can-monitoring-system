#include "dashboard_node.hpp"

#include <iostream>

#include "rpm_message.hpp"
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
    std::cout << "Dashboard state tests passed\n";
    return 0;
}
