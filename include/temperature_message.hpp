#pragma once

#include "can_frame.hpp"

inline constexpr std::uint32_t TEMPERATURE_FRAME_ID = 0x101;
inline constexpr std::uint8_t TEMPERATURE_FRAME_LENGTH = 2;

CanFrame createTemperatureFrame(std::int16_t temperature);
bool isTemperatureFrame(const CanFrame& frame);
std::int16_t readTemperature(const CanFrame& frame);
