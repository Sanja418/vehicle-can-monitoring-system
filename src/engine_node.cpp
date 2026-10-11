#include "engine_node.hpp"

#include "rpm_message.hpp"
#include "temperature_message.hpp"

EngineNode::EngineNode(std::uint16_t rpm) : rpm_(rpm) {}

CanFrame EngineNode::createFrame() const
{
    return createRpmFrame(rpm_);
}
void EngineNode::setRpm(std::uint16_t rpm)
{
    rpm_ = rpm;
}
void EngineNode::setTemperature(std::int16_t temperature)
{
    temperature_ = temperature;
}

CanFrame EngineNode::createTemperatureFrame() const
{
    return ::createTemperatureFrame(temperature_);
}
