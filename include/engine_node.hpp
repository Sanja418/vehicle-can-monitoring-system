#pragma once

#include "can_frame.hpp"

class EngineNode
{
   public:
    explicit EngineNode(std::uint16_t rpm);

    CanFrame createFrame() const;
    void setRpm(std::uint16_t rpm);

   private:
    std::uint16_t rpm_;
};
