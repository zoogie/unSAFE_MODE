.arm
.align 4
.code 32
.text

.global svcConvertVAToPA
.type svcConvertVAToPA, %function
svcConvertVAToPA:
	cpsid aif
	mov r3, r1
    mov r1, #0xf00
    orr r1, #0xff
    and r2, r0, r1
    bic r0, r1
    cmp r3, #1
    beq _convertVAToPA_write_check
    _convertVAToPA_read_check:
        mcr p15, 0, r0, c7, c8, 0    @ VA to PA translation with privileged read permission check
        b _convertVAToPA_end_check
    _convertVAToPA_write_check:
        mcr p15, 0, r0, c7, c8, 1    @ VA to PA translation with privileged write permission check
    _convertVAToPA_end_check:
    mrc p15, 0, r0, c7, c4, 0    @ read PA register
    tst r0, #1                   @ failure bit
    bic r0, r1
    addeq r0, r2
    movne r0, #0
    bx lr

.global InvalidateEntireInstructionCache
.type InvalidateEntireInstructionCache, %function
InvalidateEntireInstructionCache:
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0
	bx lr

.global CleanEntireDataCache
.type CleanEntireDataCache, %function
CleanEntireDataCache:
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 0
	bx lr

.global dsb
.type dsb, %function
dsb:
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 4
	bx lr

.global DisableInterrupts
.type DisableInterrupts, %function
DisableInterrupts:
	mrs r0, cpsr
	CPSID I
	bx lr

.global EnableInterrupts
.type EnableInterrupts, %function
EnableInterrupts:
	msr cpsr_cx, r0
	bx lr
