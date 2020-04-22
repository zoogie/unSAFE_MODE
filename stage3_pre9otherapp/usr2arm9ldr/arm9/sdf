.section .text.start
.align 4
.arm

.global _start
_start:
    nop
    mov r0, pc
    svc 0x7b
    nop

    @ Disable interrupts
    msr cpsr_cx, #0xD3

    ldr sp, =0x080c0000
    bl flushCaches

    @ Disable caches / MPU
    mrc p15, 0, r0, c1, c0, 0  @ read control register
    bic r0, #(1<<12)		   @ - instruction cache disable
    bic r0, #(1<<2)			@ - data cache disable
    bic r0, #(1<<0)			@ - mpu disable
    mcr p15, 0, r0, c1, c0, 0  @ write control register


    @ Invalidate both caches, discarding any data they may contain,
    @ then drain the write buffer
    @mov r4, #0
    @mcr p15, 0, r4, c7, c5, 0
    @mcr p15, 0, r4, c7, c6, 0
    @mcr p15, 0, r4, c7, c10, 4

    @ Give read/write access to all the memory regions
    ldr r0, =0x33333333
    mcr p15, 0, r0, c5, c0, 2 @ write data access
    mcr p15, 0, r0, c5, c0, 3 @ write instruction access

    @ Set MPU permissions and cache settings
    ldr r0, =0xFFFF001D @ ffff0000 32k  | bootrom (unprotected part)
    ldr r1, =0xFFF0001B @ fff00000 16k  | dtcm
    ldr r2, =0x01FF801D @ 01ff8000 32k  | itcm
    ldr r3, =0x08000027 @ 08000000 1M   | arm9 mem
    ldr r4, =0x10000029 @ 10000000 2M   | io mem (ARM9 / first 2MB)
    ldr r5, =0x20000035 @ 20000000 128M | fcram
    ldr r6, =0x1FF00027 @ 1FF00000 1M   | dsp / axi wram
    ldr r7, =0x1800002D @ 18000000 8M   | vram (+ 2MB)
    mov r8, #0x9
    mcr p15, 0, r0, c6, c0, 0
    mcr p15, 0, r1, c6, c1, 0
    mcr p15, 0, r2, c6, c2, 0
    mcr p15, 0, r3, c6, c3, 0
    mcr p15, 0, r4, c6, c4, 0
    mcr p15, 0, r5, c6, c5, 0
    mcr p15, 0, r6, c6, c6, 0
    mcr p15, 0, r7, c6, c7, 0
    mcr p15, 0, r8, c3, c0, 0   @ Write bufferable 0, 3
    mcr p15, 0, r8, c2, c0, 0   @ Data cacheable 0, 3
    mcr p15, 0, r8, c2, c0, 1   @ Inst cacheable 0, 3

    @ Set DTCM address and size to the default values
    ldr r1, =0xFFF0000A        @ set DTCM address and size
    mcr p15, 0, r1, c9, c1, 0  @ set the dtcm Region Register

    @ Enable caches / MPU / ITCM
    mrc p15, 0, r0, c1, c0, 0  @ read control register
    orr r0, r0, #(1<<18)       @ - ITCM enable
    orr r0, r0, #(1<<16)       @ - DTCM enable
    orr r0, r0, #(1<<13)       @ - alternate exception vectors enable
    orr r0, r0, #(1<<12)       @ - instruction cache enable
    orr r0, r0, #(1<<2)        @ - data cache enable
    orr r0, r0, #(1<<0)        @ - MPU enable
    mcr p15, 0, r0, c1, c0, 0  @ write control register

    blx main

    bl flushCaches

    @ Disable MPU
    ldr r0, =0x42078	@ alt vector select, enable itcm
    mcr p15, 0, r0, c1, c0, 0

    ldr r2, =#0x23F00000
    bx r2
