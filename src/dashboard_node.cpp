#include "dashboard_node.hpp"

#include <iostream>

#include "rpm_message.hpp"

void DashboardNode::receiveFrame(const CanFrame& frame)
{
    if (isRpmFrame(frame))
    {
        std::uint16_t rpm = readRpm(frame);
        std::cout << "Dashboard RPM: " << rpm << '\n';
    }
    else
    {
        std::cout << "Invalid RPM frame\n";
    }
}
