#pragma once

#include "can_frame.hpp"

class DashboardNode
{
   public:
    void receiveFrame(const CanFrame& frame);
};
