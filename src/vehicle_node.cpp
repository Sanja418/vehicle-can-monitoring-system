#include "vehicle_node.hpp"

#include "speed_message.hpp"

VehicleNode::VehicleNode(std::uint16_t speed) : speed_(speed) {}

void VehicleNode::setSpeed(std::uint16_t speed)
{
    speed_ = speed;
}

CanFrame VehicleNode::createFrame() const
{
    return createSpeedFrame(speed_);
}
