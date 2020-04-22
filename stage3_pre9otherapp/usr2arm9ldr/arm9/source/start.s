.section .text.start
.align 4
.arm

.global _start
_start:
    @ Elevate privileges if needed
    mrs r1, cpsr
    tst r1, #0xF
    mov r0, pc
    svceq 0x7B
    nop
    
    @ldr r0, =0x23FFFE00
    @ldr r1, =0x080CD000
    @ldmia r0, {r10,r11,r12}
    @stmia r1, {r10,r11,r12}
  

    @ Disable interrupts ASAP, clear flags
    msr cpsr_cxsf, 0xD3

    @ Change the stack pointer
    mov sp, #0x08100000

    @ Clean and invalidate the data cache, invalidate the instruction cache, drain the write buffer
    mov r4, #0
    ldr r12, =0xFFFF0830
    blx r12
    mcr p15, 0, r4, c7, c5, 0
    mcr p15, 0, r4, c7, c10, 4

    @ Disable caches / MPU
    mrc p15, 0, r0, c1, c0, 0   @ read control register
    bic r0, #(1<<12)            @ - instruction cache disable
    bic r0, #(1<<2)             @ - data cache disable
    bic r0, #(1<<0)             @ - mpu disable
    mcr p15, 0, r0, c1, c0, 0   @ write control register

    @ Give read/write access to all the memory regions
    ldr r0, =0x3333333
    mcr p15, 0, r0, c5, c0, 2 @ write data access
    mcr p15, 0, r0, c5, c0, 3 @ write instruction access

    @ Set MPU permissions and cache settings
    ldr r0, =0xFFFF001D @ ffff0000 32k  | bootrom (unprotected part)
    ldr r1, =0x01FF801D @ 01ff8000 32k  | itcm
    ldr r2, =0x08000029 @ 08000000 2M   | arm9 mem (O3DS / N3DS)
    ldr r3, =0x10000029 @ 10000000 2M   | io mem (ARM9 / first 2MB)
    ldr r4, =0x20000037 @ 20000000 256M | fcram (O3DS / N3DS)
    ldr r5, =0x1FF00027 @ 1FF00000 1M   | dsp / axi wram
    ldr r6, =0x1800002D @ 18000000 8M   | vram (+ 2MB)
    mov r7, #0
    mov r8, #5
    mcr p15, 0, r0, c6, c0, 0
    mcr p15, 0, r1, c6, c1, 0
    mcr p15, 0, r2, c6, c2, 0
    mcr p15, 0, r3, c6, c3, 0
    mcr p15, 0, r4, c6, c4, 0
    mcr p15, 0, r5, c6, c5, 0
    mcr p15, 0, r6, c6, c6, 0
    mcr p15, 0, r7, c6, c7, 0
    mcr p15, 0, r8, c3, c0, 0   @ Write bufferable 0, 2
    mcr p15, 0, r8, c2, c0, 0   @ Data cacheable 0, 2
    mcr p15, 0, r8, c2, c0, 1   @ Inst cacheable 0, 2

    @ Relocate ourselves
    adr r0, _start
    ldr r1, =__start__
    ldr r2, =__end__
    _relocate_loop:
        ldmia r0!, {r3-r10}
        stmia r1!, {r3-r10}
        cmp r1, r2
        blo _relocate_loop

    @ Enable caches / MPU / ITCM
    mrc p15, 0, r0, c1, c0, 0   @ read control register
    orr r0, r0, #(1<<18)        @ - ITCM enable
    orr r0, r0, #(1<<13)        @ - alternate exception vectors enable
    orr r0, r0, #(1<<12)        @ - instruction cache enable
    orr r0, r0, #(1<<2)         @ - data cache enable
    orr r0, r0, #(1<<0)         @ - mpu enable
    mcr p15, 0, r0, c1, c0, 0   @ write control register

    ldr lr, =_finalJump
    ldr r12, =main
    bx r12

_finalJump:
    bl flushCaches

    @ Disable MPU
    ldr r0, =0x42078            @ alt vector select, enable itcm
    mcr p15, 0, r0, c1, c0, 0
    
    @ldr r0, =0x23FFFE00
    @ldr r1, =0x080CD000
    @ldmia r1, {r10,r11,r12}
    @stmia r0, {r10,r11,r12}
    
    @ldr r1, =0x2030D000
    @ldr r2, =0x23FFFE00
    @str r1,[r2],#4
    @str r1,[r2],#4
    @str r1,[r2],#4

    bl flushCaches
    ldr r2, =#0x23F00000
    @ .word 0xe7ffdefe
    bx r2
