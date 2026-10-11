#include "temperature_message.hpp"

CanFrame createTemperatureFrame(std::int16_t temperature)
{
    CanFrame frame{};

    frame.id = TEMPERATURE_FRAME_ID;
    frame.length = TEMPERATURE_FRAME_LENGTH;

    std::uint16_t value = static_cast<std::uint16_t>(temperature);

    frame.data[0] = static_cast<std::uint8_t>(value & 0xFF);
    frame.data[1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);

    return frame;
}

bool isTemperatureFrame(const CanFrame& frame)
{
    return frame.id == TEMPERATURE_FRAME_ID && frame.length == TEMPERATURE_FRAME_LENGTH;
}

std::int16_t readTemperature(const CanFrame& frame)
{
    std::int32_t value =
        static_cast<std::int32_t>(frame.data[0]) | (static_cast<std::int32_t>(frame.data[1]) << 8);

    if (value >= 32768)
    {
        value -= 65536;
    }

    return static_cast<std::int16_t>(value);
}
