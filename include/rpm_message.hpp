#pragma once

#include "can_frame.hpp"

CanFrame createRpmFrame(std::uint16_t rpm);
std::uint16_t readRpm(const CanFrame& frame);

bool isRpmFrame(const CanFrame& frame);
