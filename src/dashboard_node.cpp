#include "dashboard_node.hpp"

#include <iostream>

#include "rpm_message.hpp"
#include "speed_message.hpp"
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
    else if (isSpeedFrame(frame))
    {
        speed_ = readSpeed(frame);
        hasSpeed_ = true;
        lastSpeedTime_ = now;
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
        if (isEngineOverheated(timeout))
        {
            std::cout << "WARNING: engine temperature too high\n";
        }
    }
    if (!hasSpeed())
    {
        std::cout << "Dashboard: no speed data\n";
    }
    else if (!isSpeedFresh(timeout))
    {
        std::cout << "Dashboard: speed data stale\n";
    }
    else
    {
        std::cout << "Dashboard speed: " << getSpeed() << " km/h\n";
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
bool DashboardNode::isEngineOverheated(std::chrono::milliseconds timeout,
                                       std::chrono::steady_clock::time_point now) const
{
    return isTemperatureFresh(timeout, now) && temperature_ >= ENGINE_OVERHEAT_THRESHOLD_C;
}
std::uint16_t DashboardNode::getSpeed() const
{
    return speed_;
}

bool DashboardNode::hasSpeed() const
{
    return hasSpeed_;
}

bool DashboardNode::isSpeedFresh(std::chrono::milliseconds timeout,
                                 std::chrono::steady_clock::time_point now) const
{
    if (!hasSpeed_)
    {
        return false;
    }

    return now - lastSpeedTime_ < timeout;
}
