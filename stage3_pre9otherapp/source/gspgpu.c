#include <string.h>
#include "gspgpu.h"
#include "libctru/svc.h"

Result GSP_ImportDisplayCaptureInfo(Handle* handle, GSPGPU_CaptureInfo *captureinfo)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x00180000; //request header code

	Result ret=0;
	if((ret=svcSendSyncRequest(*handle)))return ret;

	ret = cmdbuf[1];

	if(ret==0)
	{
		memcpy(captureinfo, &cmdbuf[2], 0x20);
	}

	return ret;
}

u8 *GSP_GetScreenFBADR(u8 screen)
{
	GSPGPU_CaptureInfo capinfo;
	u32 ptr;
	u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	Handle* gspHandle=(Handle*)paramblk[0x58>>2];
	
	if(GSP_ImportDisplayCaptureInfo(gspHandle, &capinfo)!=0)return NULL;

	switch(screen)
	{
		case 0: //TOP-LEFT
		ptr = (u32)capinfo.screencapture[0].framebuf0_vaddr;
		break;
		
		case 1: //TOP-RIGHT
		ptr = (u32)capinfo.screencapture[0].framebuf1_vaddr;
		break;
		
		case 2: // BOTTOM
		ptr = (u32)capinfo.screencapture[1].framebuf0_vaddr;
		break;
	}
	if(ptr>=0x1f000000 && ptr<0x1f600000)return NULL;//Don't return a ptr to VRAM if framebuf is located there, since writing there will only crash.

	return (u8*)ptr;
}

Result GSP_FlushDCache(u32* addr, u32 size)
{
	Result (*_GSP_FlushDCache)(u32* addr, u32 size);
	u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	_GSP_FlushDCache = (void*)paramblk[0x20>>2];
	return _GSP_FlushDCache(addr, size);
}

Result GSP_WriteHWRegs(u32 regAddr, u32 *data, u8 size)
{
	u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	Handle* gspHandle = (Handle*)paramblk[0x58>>2];
	
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = 0x10082;
	cmdbuf[1] = regAddr;
	cmdbuf[2] = size;
	cmdbuf[3] = (size << 14) | 2;
	cmdbuf[4] = (u32)data;

	Result ret=0;
	if((ret = svcSendSyncRequest(*gspHandle)))return ret;

	return cmdbuf[1];
}

Result GSP_ReadHWRegs(u32 regAddr, u32 *data, u8 size)
{
	u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	Handle* gspHandle = (Handle*)paramblk[0x58>>2];
	
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = 0x40080;
	cmdbuf[1] = regAddr;
	cmdbuf[2] = size;
	cmdbuf[0x40] = (size << 14) | 2;
	cmdbuf[0x41] = (u32)data;

	Result ret=0;
	if((ret = svcSendSyncRequest(*gspHandle)))return ret;

	return cmdbuf[1];
}

Result GSP_InvalidateDataCache(void* adr, u32 size)
{
	u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	Handle* gspHandle = (Handle*)paramblk[0x58>>2];
	
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = 0x90082;
	cmdbuf[1] = (u32)adr;
	cmdbuf[2] = size;
	cmdbuf[3] = 0;
	cmdbuf[4] = 0xFFFF8001;

	Result ret=0;
	if((ret=svcSendSyncRequest(*gspHandle)))return ret;

	return cmdbuf[1];
}

Result GSP_ReleaseRight(Handle handle)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x170000; //request header code

	Result ret=0;
	if((ret=svcSendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result GSP_SetBufferSwap(Handle handle, u32 screenid, GSPGPU_FramebufferInfo framebufinfo)
{
	Result ret=0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x00050200;
	cmdbuf[1] = screenid;
	memcpy(&cmdbuf[2], &framebufinfo, sizeof(GSPGPU_FramebufferInfo));
	
	if((ret=svcSendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}