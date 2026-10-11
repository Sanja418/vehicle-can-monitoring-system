#pragma once

#include "can_frame.hpp"

class EngineNode
{
   public:
    explicit EngineNode(std::uint16_t rpm);

    CanFrame createFrame() const;
    void setRpm(std::uint16_t rpm);
    void setTemperature(std::int16_t temperature);
    CanFrame createTemperatureFrame() const;

   private:
    std::uint16_t rpm_;
    std::int16_t temperature_{0};
};
