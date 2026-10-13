#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

mkdir -p build/firmware/can_demo

build_firmware()
{
    arm-none-eabi-g++ \
        -mcpu=cortex-m4 \
        -mthumb \
        -std=c++20 \
        -O0 \
        -g \
        -ffreestanding \
        -fno-exceptions \
        -fno-rtti \
        -fno-unwind-tables \
        -fno-asynchronous-unwind-tables \
        -nostdlib \
        -T firmware/can_demo/linker.ld \
        firmware/can_demo/startup.s \
        firmware/can_demo/main.cpp \
        "$@"
}

build_firmware \
    -o build/firmware/can_demo/sender.elf

build_firmware \
    -DCAN_RECEIVER \
    -o build/firmware/can_demo/receiver.elf

arm-none-eabi-size \
    build/firmware/can_demo/sender.elf \
    build/firmware/can_demo/receiver.elf
