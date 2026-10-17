#pragma once

#include "can_frame.hpp"

bool initCan();
bool sendCanFrame(const CanFrame& frame);
bool receiveCanFrame(CanFrame& frame);
