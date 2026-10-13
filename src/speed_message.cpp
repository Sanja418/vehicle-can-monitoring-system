#include "speed_message.hpp"

CanFrame createSpeedFrame(std::uint16_t speed)
{
    CanFrame frame{};

    frame.id = SPEED_FRAME_ID;
    frame.length = SPEED_FRAME_LENGTH;
    frame.data[0] = static_cast<std::uint8_t>(speed & 0xFF);
    frame.data[1] = static_cast<std::uint8_t>((speed >> 8) & 0xFF);

    return frame;
}

bool isSpeedFrame(const CanFrame& frame)
{
    return frame.id == SPEED_FRAME_ID && frame.length == SPEED_FRAME_LENGTH;
}

std::uint16_t readSpeed(const CanFrame& frame)
{
    return static_cast<std::uint16_t>(frame.data[0]) |
           (static_cast<std::uint16_t>(frame.data[1]) << 8);
}
