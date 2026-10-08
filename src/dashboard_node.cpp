#include "dashboard_node.hpp"

#include <iostream>

#include "rpm_message.hpp"

void DashboardNode::receiveFrame(const CanFrame& frame)
{
    if (isRpmFrame(frame))
    {
        rpm_ = readRpm(frame);
        hasRpm_ = true;
        lastRpmTime_ = std::chrono::steady_clock::now();
    }
    else
    {
        std::cout << "Invalid RPM frame\n";
    }
}

std::uint16_t DashboardNode::getRpm() const
{
    return rpm_;
}
bool DashboardNode::hasRpm() const
{
    return hasRpm_;
}
bool DashboardNode::isRpmFresh(std::chrono::milliseconds timeout) const
{
    if (!hasRpm_)
    {
        return false;
    }

    auto elapsed = std::chrono::steady_clock::now() - lastRpmTime_;

    return elapsed < timeout;
}
void DashboardNode::printStatus(std::chrono::milliseconds timeout) const
{
    if (!hasRpm())
    {
        std::cout << "Dashboard: no RPM data\n";
    }
    else if (!isRpmFresh(timeout))
    {
        std::cout << "Dashboard: RPM data stale\n";
    }
    else
    {
        std::cout << "Dashboard RPM: " << getRpm() << '\n';
    }
}
