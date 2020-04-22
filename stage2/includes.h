#define ROP_POPPC 0x001000c4
#define POP_R1PC 0x001c5614
#define POP_R3PC 0x00110ce0
#define POP_R2R6PC 0x0019d820
#define POP_R4LR_BXR1 0x0014f844
#define POP_R4R8LR_BXR2 0x001464c4
#define POP_R4R5R6PC 0x00100120
#define POP_R4FPPC 0x0010dbc0
#define POP_R4R8PC 0x0010dd48

#define ROP_STR_R1TOR0 0x00112f3c
#define ROP_STR_R0TOR1 0x00196fb8
#define ROP_LDR_R0FROMR0 0x001878d8
#define ROP_ADDR0_TO_R1 0x001928e4

#define MEMCPY 0x001c5ef4

#define svcSleepThread 0x001955c0

#define GSPGPU_FlushDataCache 0x0022855c
#define GSPGPU_SERVHANDLEADR 0x00260e3c

#define IFile_Read 0x001bd8e8
#define IFile_Write 0x001bd984

#define ROP_POPR3_ADDSPR3_POPPC 0x001000bc
#define POP_R0PC 0x00100874
#define ROP_LDRR1R1_STRR1R0 0x001b746c
//      FAIL:                           #define POP_R5R6PC
#define ROP_CMPR0R1_ALT0 0x0012ec00
#define MEMSET32_OTHER 0x001c6120
#define svcControlMemory 0x00190c9c
#define ROP_COND_THROWFATALERR_ALT0 0x00111054
#define svcCreateThread 0x00201478
#define svcConnectToPort 0x0018a0fc
#define svcGetProcessId 0x001a5b00
#define THROWFATALERR_IPC 0x0020c8d8
//      FAIL:                           #define SRV_GETSERVICEHANDLE
//      FAIL:                           #define CFGIPC_SecureInfoGetRegion
#define GXLOW_CMD4  0x00230c30
//      FAIL:                           #define GSP_SHAREDMEM_SETUPFRAMEBUF
#define GSPTHREAD_OBJECTADDR 0x002F0DE0
#define FS_MountSdmc 0x1A3C04
//      FAIL:                           #define FS_MountSavedata
#define IFile_Open 0x001bdb84           
#define IFile_Close 0x001bd96c
//      FAIL:                           #define IFile_Seek
