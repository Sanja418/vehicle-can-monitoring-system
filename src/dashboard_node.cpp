#include "dashboard_node.hpp"

#include <iostream>

#include "rpm_message.hpp"
#include "temperature_message.hpp"

void DashboardNode::receiveFrame(const CanFrame& frame, std::chrono::steady_clock::time_point now)
{
    if (isRpmFrame(frame))
    {
        rpm_ = readRpm(frame);
        hasRpm_ = true;
        lastRpmTime_ = now;
    }
    else if (isTemperatureFrame(frame))
    {
        temperature_ = readTemperature(frame);
        hasTemperature_ = true;
        lastTemperatureTime_ = now;
    }
    else
    {
        std::cout << "Invalid or unsupported CAN frame\n";
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
bool DashboardNode::isRpmFresh(std::chrono::milliseconds timeout,
                               std::chrono::steady_clock::time_point now) const
{
    if (!hasRpm_)
    {
        return false;
    }

    return now - lastRpmTime_ < timeout;
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
    if (!hasTemperature())
    {
        std::cout << "Dashboard: no temperature data\n";
    }
    else if (!isTemperatureFresh(timeout))
    {
        std::cout << "Dashboard: temperature data stale\n";
    }
    else
    {
        std::cout << "Dashboard temperature: " << getTemperature() << " C\n";
    }
}
std::int16_t DashboardNode::getTemperature() const
{
    return temperature_;
}

bool DashboardNode::hasTemperature() const
{
    return hasTemperature_;
}
bool DashboardNode::isTemperatureFresh(std::chrono::milliseconds timeout,
                                       std::chrono::steady_clock::time_point now) const
{
    if (!hasTemperature_)
    {
        return false;
    }

    return now - lastTemperatureTime_ < timeout;
}
