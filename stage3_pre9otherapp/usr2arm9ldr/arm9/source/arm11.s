.section .rodata.arm11FirmlaunchStub, "a", %progbits
.align 4
.global arm11FirmlaunchStub
arm11FirmlaunchStub: .incbin "../build/arm11_firmlaunch_stub.bin"
arm11FirmlaunchStubEnd:


.align 4
.global arm11FirmlaunchStubSize
arm11FirmlaunchStubSize: .word arm11FirmlaunchStubEnd - arm11FirmlaunchStub
