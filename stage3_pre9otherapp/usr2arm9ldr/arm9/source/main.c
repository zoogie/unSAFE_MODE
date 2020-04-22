#include "types.h"
#include "PXI.h"
#include "arm11.h"
#include "petitfs/pff.h"

#define CFG11_SHAREDWRAM_32K_DATA(i)    (*(vu8 *)(0x10140000 + i))
#define CFG11_SHAREDWRAM_32K_CODE(i)    (*(vu8 *)(0x10140008 + i))
#define CFG11_DSP_CNT                   (*(vu8 *)0x10141230)

struct fb {
     u8 *top_left;
     u8 *top_right;
     u8 *bottom;
} ;

static const struct fb fbs[2] =
{
    {
        .top_left  = (u8 *)0x18300000,
        .top_right = (u8 *)0x18300000,
        .bottom    = (u8 *)0x18346500,
    },
    {
        .top_left  = (u8 *)0x18400000,
        .top_right = (u8 *)0x18400000,
        .bottom    = (u8 *)0x18446500,
    },
};

static FATFS sdFs;

static bool mountFs(void)
{
    return pf_mount(&sdFs) == FR_OK;
}

static bool fileRead(void *dest, const char *path, u32 maxSize, u32 offset)
{
    FRESULT result = FR_OK;
    u32 ret = 0;

    if(pf_open(path) != FR_OK) return false;
    
    pf_lseek(offset);

    result = pf_read(dest, maxSize, (unsigned int *)&ret);

    return result == FR_OK && ret != 0;
}

static bool readPayload(void)
{
    return mountFs() && fileRead((void *)0x23F00000, "/usm.bin", 0x100000, 0x10000);
}

static void resetDSPAndSharedWRAMConfig(void)
{
    CFG11_DSP_CNT = 2; // PDN_DSP_CNT
    for(volatile int i = 0; i < 10; i++);

    CFG11_DSP_CNT = 3;
    for(volatile int i = 0; i < 10; i++);

    for(int i = 0; i < 8; i++)
        CFG11_SHAREDWRAM_32K_DATA(i) = i << 2; // disabled, master = arm9

    for(int i = 0; i < 8; i++)
        CFG11_SHAREDWRAM_32K_CODE(i) = i << 2; // disabled, master = arm9

    for(int i = 0; i < 8; i++)
        CFG11_SHAREDWRAM_32K_DATA(i) = 0x80 | (i << 2); // enabled, master = arm9

    for(int i = 0; i < 8; i++)
        CFG11_SHAREDWRAM_32K_CODE(i) = 0x80 | (i << 2); // enabled, master = arm9
}

static void doFirmlaunch(void)
{
    bool payloadRead;

    while(PXIReceiveWord() != 0x44836);
    PXISendWord(0x964536);
    while(PXIReceiveWord() != 0x44837);
    PXIReceiveWord(); // High FIRM titleID
    PXIReceiveWord(); // Low FIRM titleID
    resetDSPAndSharedWRAMConfig();

    while(PXIReceiveWord() != 0x44846);

    for(int i=0;i<5;i++){
	payloadRead = readPayload();
	if(payloadRead==true) break;
    }

    *(vu32 *)0x1FFFFFF8 = 0;
    memcpy((void *)0x1FFFF400, arm11FirmlaunchStub, arm11FirmlaunchStubSize);
    if(payloadRead)
        *(vu32 *)0x1FFFFFFC = 0x1FFFF400;
    else
    {
        *(vu32 *)0x1FFFFFFC = 0x1FFFF404; // fill the screens with red
	*(u32*)NULL=42;
        while(true);
    }
}

static void patchSvcReplyAndReceive11(void)
{
    /*
       Basically, we're patching svc 0x4F's contents to svcKernelSetState(0, (u64)<dontcare>).
       Assumption: kernel .text is in the same 64KB as the first SVCs.
    */
    u32 *off, *svcTable;

    for(off = (u32 *)0x1FF80000; off[0] != 0xF96D0513 || off[1] != 0xE94D6F00; off++);
    for(; *off != 0; off++);
    svcTable = off;

    u32 baseAddr = svcTable[1] & ~0xFFFF;
    u32 *patch = (u32 *)(0x1FF80000 + svcTable[0x4F] - baseAddr);
    patch[0] = 0xE3A00000;
    patch[1] = 0xE51FF004;
    patch[2] = svcTable[0x7C];;
}

void main(void)
{
    bool payloadRead;
    //memcpy((void *)0x23FFFE00, fbs, 2 * sizeof(struct fb));
    for(int i=0;i<5;i++){
	payloadRead = readPayload();
	if(payloadRead==true) break;
    }
    //patchSvcReplyAndReceive11();
    //doFirmlaunch();
    //*(u32*)NULL=42;
}
