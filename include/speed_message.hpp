#pragma once

#include "can_frame.hpp"

inline constexpr std::uint32_t SPEED_FRAME_ID = 0x102;
inline constexpr std::uint8_t SPEED_FRAME_LENGTH = 2;

CanFrame createSpeedFrame(std::uint16_t speed);
bool isSpeedFrame(const CanFrame& frame);
std::uint16_t readSpeed(const CanFrame& frame);
