#include "engine_node.hpp"

#include "rpm_message.hpp"

EngineNode::EngineNode(std::uint16_t rpm) : rpm_(rpm) {}

CanFrame EngineNode::createFrame() const
{
    return createRpmFrame(rpm_);
}
void EngineNode::setRpm(std::uint16_t rpm)
{
    rpm_ = rpm;
}
