#pragma once

namespace stm32
{
inline constexpr unsigned int RCC_APB1ENR = 0x40023840;

inline constexpr unsigned int UART4_SR = 0x40004C00;
inline constexpr unsigned int UART4_DR = 0x40004C04;
inline constexpr unsigned int UART4_BRR = 0x40004C08;
inline constexpr unsigned int UART4_CR1 = 0x40004C0C;
inline constexpr unsigned int TIM2_CR1 = 0x40000000;
inline constexpr unsigned int TIM2_SR = 0x40000010;
inline constexpr unsigned int TIM2_EGR = 0x40000014;
inline constexpr unsigned int TIM2_CNT = 0x40000024;
inline constexpr unsigned int TIM2_PSC = 0x40000028;
inline constexpr unsigned int TIM2_ARR = 0x4000002C;
inline constexpr unsigned int CAN1_MCR = 0x40006400;
inline constexpr unsigned int CAN1_MSR = 0x40006404;
inline constexpr unsigned int CAN1_TSR = 0x40006408;
inline constexpr unsigned int CAN1_RF0R = 0x4000640C;
inline constexpr unsigned int CAN1_BTR = 0x4000641C;

inline constexpr unsigned int CAN1_TI0R = 0x40006580;
inline constexpr unsigned int CAN1_TDT0R = 0x40006584;
inline constexpr unsigned int CAN1_TDL0R = 0x40006588;
inline constexpr unsigned int CAN1_TDH0R = 0x4000658C;

inline constexpr unsigned int CAN1_RI0R = 0x400065B0;
inline constexpr unsigned int CAN1_RDT0R = 0x400065B4;
inline constexpr unsigned int CAN1_RDL0R = 0x400065B8;
inline constexpr unsigned int CAN1_RDH0R = 0x400065BC;

inline constexpr unsigned int CAN1_FMR = 0x40006600;
inline constexpr unsigned int CAN1_FM1R = 0x40006604;
inline constexpr unsigned int CAN1_FS1R = 0x4000660C;
inline constexpr unsigned int CAN1_FFA1R = 0x40006614;
inline constexpr unsigned int CAN1_FA1R = 0x4000661C;
inline constexpr unsigned int CAN1_F0R1 = 0x40006640;
inline constexpr unsigned int CAN1_F0R2 = 0x40006644;
}  // namespace stm32
