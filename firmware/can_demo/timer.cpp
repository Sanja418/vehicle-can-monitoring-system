#include "timer.hpp"

#include "register_access.hpp"
#include "stm32_registers.hpp"

void initTimer()
{
    registerAt(stm32::RCC_APB1ENR) = registerAt(stm32::RCC_APB1ENR) | 1u;

    registerAt(stm32::TIM2_CR1) = 0u;

    // Renode platform: 10 MHz / 10000 = 1000 ticks per second.
    registerAt(stm32::TIM2_PSC) = 9999u;
    registerAt(stm32::TIM2_ARR) = 0xFFFFFFFFu;

    registerAt(stm32::TIM2_EGR) = 1u;
    registerAt(stm32::TIM2_SR) = 0u;
    registerAt(stm32::TIM2_CNT) = 0u;

    registerAt(stm32::TIM2_CR1) = 1u;
}

unsigned int readMilliseconds()
{
    return registerAt(stm32::TIM2_CNT);
}
