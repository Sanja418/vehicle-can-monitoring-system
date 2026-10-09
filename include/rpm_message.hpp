#pragma once

#include "can_frame.hpp"
inline constexpr std::uint32_t RPM_FRAME_ID = 0x100;
inline constexpr std::uint8_t RPM_FRAME_LENGTH = 2;
CanFrame createRpmFrame(std::uint16_t rpm);
std::uint16_t readRpm(const CanFrame& frame);

bool isRpmFrame(const CanFrame& frame);
