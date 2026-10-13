#pragma once

#include "can_frame.hpp"

class VehicleNode
{
   public:
    explicit VehicleNode(std::uint16_t speed);

    void setSpeed(std::uint16_t speed);
    CanFrame createFrame() const;

   private:
    std::uint16_t speed_;
};
