.syntax unified
.cpu cortex-m4
.thumb

.section .isr_vector, "a", %progbits
.word _estack
.word Reset_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word 0
.word 0
.word 0
.word 0
.word Default_Handler
.word Default_Handler
.word 0
.word Default_Handler
.word Default_Handler

.section .text.Reset_Handler, "ax", %progbits
.global Reset_Handler
.type Reset_Handler, %function
.thumb_func

Reset_Handler:
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata

copy_data:
    cmp r1, r2
    bcs clear_bss_setup
    ldr r3, [r0], #4
    str r3, [r1], #4
    b copy_data

clear_bss_setup:
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0

clear_bss:
    cmp r1, r2
    bcs call_main
    str r3, [r1], #4
    b clear_bss

call_main:
    bl main

halt:
    b halt

.size Reset_Handler, . - Reset_Handler

.section .text.Default_Handler, "ax", %progbits
.type Default_Handler, %function
.thumb_func

Default_Handler:
    b Default_Handler

.size Default_Handler, . - Default_Handler
