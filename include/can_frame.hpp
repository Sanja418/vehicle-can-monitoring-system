#pragma once

#include <array>
#include <cstdint>

struct CanFrame
{
    std::uint32_t id{};
    std::uint8_t length{};
    std::array<std::uint8_t, 8> data{};
};
