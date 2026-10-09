#include "rpm_message.hpp"

CanFrame createRpmFrame(std::uint16_t rpm)
{
    CanFrame frame{};

    frame.id = RPM_FRAME_ID;
    frame.length = RPM_FRAME_LENGTH;
    frame.data[0] = static_cast<std::uint8_t>(rpm & 0xFF);
    frame.data[1] = static_cast<std::uint8_t>((rpm >> 8) & 0xFF);

    return frame;
}

std::uint16_t readRpm(const CanFrame& frame)
{
    return static_cast<std::uint16_t>(frame.data[0]) |
           (static_cast<std::uint16_t>(frame.data[1]) << 8);
}

bool isRpmFrame(const CanFrame& frame)
{
    return frame.id == RPM_FRAME_ID && frame.length == RPM_FRAME_LENGTH;
}
