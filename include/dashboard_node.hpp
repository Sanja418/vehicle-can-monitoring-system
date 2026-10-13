#pragma once
#include <chrono>

#include "can_frame.hpp"

class DashboardNode
{
   public:
    void receiveFrame(const CanFrame& frame,
                      std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now());

    std::uint16_t getRpm() const;
    bool hasRpm() const;
    bool isRpmFresh(std::chrono::milliseconds timeout, std::chrono::steady_clock::time_point now =
                                                           std::chrono::steady_clock::now()) const;
    void printStatus(std::chrono::milliseconds timeout) const;
    std::int16_t getTemperature() const;
    bool hasTemperature() const;

    bool isTemperatureFresh(
        std::chrono::milliseconds timeout,
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now()) const;

   private:
    std::uint16_t rpm_{0};
    bool hasRpm_{false};
    std::chrono::steady_clock::time_point lastRpmTime_{};
    std::int16_t temperature_{0};
    bool hasTemperature_{false};
    std::chrono::steady_clock::time_point lastTemperatureTime_{};
};
