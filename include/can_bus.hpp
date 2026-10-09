#pragma once

#include <queue>

#include "can_frame.hpp"

class CanBus
{
   public:
    void send(const CanFrame& frame);
    bool receive(CanFrame& frame);

   private:
    std::queue<CanFrame> frames_;
};
