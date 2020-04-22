.section .rodata.arm9Payload, "a", %progbits
.align 4

arm9Payload: .incbin "build/arm9.bin"
arm9PayloadEnd:

.align 4

.section .text.getArm9Payload, "ax", %progbits
.arm
.align 2
.global getArm9Payload
.type   getArm9Payload, %function
getArm9Payload:
    ldr r1, =(arm9PayloadEnd - arm9Payload)
    str r1, [r0]
    ldr r0, [pc, #(_offset - . - 8)]
    add r0, pc
    bx lr
_offset: .word arm9Payload - (getArm9Payload + 12 + 8)
