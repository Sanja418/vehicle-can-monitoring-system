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

    std::cout << "Dashboard state tests passed\n";
    return 0;
}
