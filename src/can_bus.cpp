#include "can_bus.hpp"

void CanBus::send(const CanFrame& frame)
{
    frames_.push(frame);
}

bool CanBus::receive(CanFrame& frame)
{
    if (frames_.empty())
    {
        return false;
    }

    frame = frames_.front();
    frames_.pop();

    return true;
}
