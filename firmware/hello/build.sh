#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

mkdir -p build/firmware/hello

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
    -T firmware/hello/linker.ld \
    firmware/hello/startup.s \
    firmware/hello/main.cpp \
    -o build/firmware/hello/hello.elf

arm-none-eabi-size build/firmware/hello/hello.elf
