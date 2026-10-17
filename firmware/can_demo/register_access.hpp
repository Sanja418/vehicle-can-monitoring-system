#pragma once

inline volatile unsigned int& registerAt(unsigned int address)
{
    return *reinterpret_cast<volatile unsigned int*>(address);
}
