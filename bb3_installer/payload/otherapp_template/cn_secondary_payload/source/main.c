#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctr/types.h>
#include <ctr/srv.h>
#include <ctr/svc.h>
#include <ctr/APT.h>
#include <ctr/FS.h>
#include <ctr/GSP.h>
#include "text.h"

#define HID_PAD (*(vu32*)0x1000001C)
#define GSPGPU_FlushDataCache 0x0013e46c
#define GSPGPU_SERVHANDLEADR 0x002993c4
#define VRAM ((u32)0x31000000)
#define workbuf (u8*)0x00690000
#define slot1 (u8*)(0x00682000+0x1F000)
#define cfgHandle (u32*)0x00600000
#define HAXX 0x58584148
//u8 *top_ptr=(u8*)1;

typedef enum
{
	PAD_A = (1<<0),
	PAD_B = (1<<1),
	PAD_SELECT = (1<<2),
	PAD_START = (1<<3),
	PAD_RIGHT = (1<<4),
	PAD_LEFT = (1<<5),
	PAD_UP = (1<<6),
	PAD_DOWN = (1<<7),
	PAD_R = (1<<8),
	PAD_L = (1<<9),
	PAD_X = (1<<10),
	PAD_Y = (1<<11)
}PAD_KEY;

#define BIT(n) (1U<<(n))
typedef enum
{
	IPC_BUFFER_R  = BIT(1),                     ///< Readable
	IPC_BUFFER_W  = BIT(2),                     ///< Writable
	IPC_BUFFER_RW = IPC_BUFFER_R | IPC_BUFFER_W ///< Readable and Writable
} IPC_BufferRights;

static inline u32 IPC_Desc_Buffer(size_t size, IPC_BufferRights rights)
{
	return (size << 4) | 0x8 | rights;
}


int _strlen(const char* str)
{
	int l=0;
	while(*(str++))l++;
	return l;
}

void _strcpy(char* dst, char* src)
{
	while(*src)*(dst++)=*(src++);
	*dst=0x00;
}

void _strappend(char* str1, char* str2)
{
	_strcpy(&str1[_strlen(str1)], str2);
}

Result _srv_RegisterClient(Handle* handleptr)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x10002; //request header code
	cmdbuf[1]=0x20;

	Result ret=0;
	if((ret=svc_sendSyncRequest(*handleptr)))return ret;

	return cmdbuf[1];
}

Result _initSrv(Handle* srvHandle)
{
	Result ret=0;
	if(svc_connectToPort(srvHandle, "srv:"))return ret;
	return _srv_RegisterClient(srvHandle);
}

Result _srv_getServiceHandle(Handle* handleptr, Handle* out, char* server)
{
	u8 l=_strlen(server);
	if(!out || !server || l>8)return -1;

	u32* cmdbuf=getThreadCommandBuffer();

	cmdbuf[0]=0x50100; //request header code
	_strcpy((char*)&cmdbuf[1], server);
	cmdbuf[3]=l;
	cmdbuf[4]=0x0;

	Result ret=0;
	if((ret=svc_sendSyncRequest(*handleptr)))return ret;

	*out=cmdbuf[3];

	return cmdbuf[1];
}

Result _GSPGPU_ImportDisplayCaptureInfo(Handle* handle, GSP_CaptureInfo *captureinfo)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x00180000; //request header code

	Result ret=0;
	if((ret=svc_sendSyncRequest(*handle)))return ret;

	ret = cmdbuf[1];

	if(ret==0)
	{
		memcpy(captureinfo, &cmdbuf[2], 0x20);
	}

	return ret;
}

u8 *GSP_GetTopFBADR()
{
	GSP_CaptureInfo capinfo;
	u32 ptr;

	//u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	Handle* gspHandle=(Handle*)GSPGPU_SERVHANDLEADR;
	//#define gspHandle (Handle*)0x002993c4


	if(_GSPGPU_ImportDisplayCaptureInfo(gspHandle, &capinfo)!=0)return NULL;

	ptr = (u32)capinfo.screencapture[0].framebuf0_vaddr;
	if(ptr>=0x1f000000 && ptr<0x1f600000)return NULL;//Don't return a ptr to VRAM if framebuf is located there, since writing there will only crash.


	return (u8*)ptr;
}

Result GSP_FlushDCache(u32* addr, u32 size)
{
	
	//Handle* gspHandle=(Handle*)GSPGPU_SERVHANDLEADR;
	Result (*_GSP_FlushDCache)(u32* addr, u32 size);
	//u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	_GSP_FlushDCache=(void*)GSPGPU_FlushDataCache;
	return _GSP_FlushDCache(addr, size);
}



const u8 hexTable[]=
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void hex2str(char* out, u32 val)
{
	int i;
	for(i=0;i<8;i++){out[7-i]=hexTable[val&0xf];val>>=4;}
	out[8]=0x00;
}

void renderString(char* str, int x, int y)
{
	//u8 *ptr = GSP_GetTopFBADR();
	u8 *ptr=(u8*)VRAM;
	if(ptr==NULL)return;
	drawString(ptr,str,x,y);
	GSP_FlushDCache((u32*)ptr, 240*400*3);
}

void centerString(char* str, int y)
{
	//u8 *ptr = GSP_GetTopFBADR();
	u8 *ptr=(u8*)VRAM;
	//volatile u32 lol=*(u32*)0;
	int x=200-(_strlen(str)*4);
	if(ptr==NULL)return;
	drawString(ptr,str,x,y);
	GSP_FlushDCache((u32*)ptr, 240*400*3);
}

void drawHex(u32 val, int x, int y)
{
	char str[9];

	hex2str(str,val);
	renderString(str,x,y);
}

void clearScreen(u8 shade)
{
	//u8 *ptr = GSP_GetTopFBADR();
	u8 *ptr=(u8*)VRAM;
	if(ptr==NULL)return;
	memset(ptr, shade, 240*400*3);
	GSP_FlushDCache((u32*)ptr, 240*400*3);
}

void drawTitleScreen(char* str)
{
	clearScreen(0x00);
	centerString("Bannerbomb3 by zoogie",0);
	centerString("https://3ds.hacks.guide/",10);
	centerString("Help: https://discord.gg/C29hYvh",20);
	renderString(str, 0, 70);
}

Result NS_RebootSystem(Handle handle, u32 command)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = command; //0x160000 = reboot   0xE0000 = shutdown
	
	if((ret = svc_sendSyncRequest(handle)))return ret;

	return (Result)cmdbuf[1];
}

Result _GSPGPU_SetBufferSwap(Handle handle, u32 screenid, GSP_FramebufferInfo framebufinfo)
{
	Result ret=0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x00050200;
	cmdbuf[1] = screenid;
	memcpy(&cmdbuf[2], &framebufinfo, sizeof(GSP_FramebufferInfo));
	
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result _GSPGPU_SetLcdForceBlack(Handle handle, u8 flags)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0xB0040; // 0xB0040
	cmdbuf[1]=flags;

	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result _CFG_GetConfigInfoBlk4(u32 blkID, u8* outData)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x4010082; // 0x4010082
	cmdbuf[1] = 0xC00;
	cmdbuf[2] = blkID;
	cmdbuf[3] = IPC_Desc_Buffer(0xC00,IPC_BUFFER_W);
	cmdbuf[4] = (u32)outData;

	if((ret=svc_sendSyncRequest(*cfgHandle)))return ret;

	return (Result)cmdbuf[1];
}

Result _CFG_SetConfigInfoBlk4(u32 blkID, u8* inData)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] =  0x4020082; // 0x4020082
	cmdbuf[1] = blkID;
	cmdbuf[2] = 0xC00;
	cmdbuf[3] = IPC_Desc_Buffer(0xC00,IPC_BUFFER_R);
	cmdbuf[4] = (u32)inData;

	if((ret=svc_sendSyncRequest(*cfgHandle)))return ret;

	return (Result)cmdbuf[1];
}


Result _CFG_UpdateConfigSavegame()
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x4030000; // changed from 0x8030000 to be compatible with both cfg:i and cfg:s

	if((ret=svc_sendSyncRequest(*cfgHandle)))return ret;

	return (Result)cmdbuf[1];
}

Result check_slots(){
	int y=70;
	renderString("Slot Status:  ", 0, y);
	for(int i=0; i<3; i++){
		y+=10;
		drawHex(i+1, 0, y);
		 _CFG_GetConfigInfoBlk4(0x80000+i, workbuf);
		if(*(u32*)(workbuf+0x420) == HAXX){
			renderString("  Haxx", 64, y);
		}
		else{
			renderString("  User", 64, y);
		}
	}
	
	return 0;
}

Result restore_slots(){
	for(int i=0; i<3; i++){
		memset(workbuf, 0, 0xC00);
		_CFG_GetConfigInfoBlk4(0x80000+i, workbuf);
		if(*(u32*)(workbuf+0x420) == HAXX){
			memcpy(workbuf, workbuf+0x500, 0x500); //restore backup slot to wifi slot
			memset(workbuf+0x500, 0, 0x500);       //clear slot backup to zeros
			_CFG_SetConfigInfoBlk4(0x80000+i, workbuf); //commit workbuf to slot
		}
		else{
			//pass
		}
	}
	 _CFG_UpdateConfigSavegame();
	
	return 0;
}

Result inject_slots(){

	for(int i=0; i<3; i++){
		memset(workbuf, 0, 0xC00);
		_CFG_GetConfigInfoBlk4(0x80000+i, workbuf);
		if(*(u32*)(workbuf+0x420) == HAXX){
			//pass
		}
		else{
			memcpy(workbuf+0x500, workbuf, 0x500); //backup user slot to slot+0x500
			memcpy(workbuf, slot1, 0x500);         //write slot1 to workbuf
			_CFG_SetConfigInfoBlk4(0x80000+i, workbuf); //commit workbuf to slot
		}
	}
	 _CFG_UpdateConfigSavegame();
	 
	 return 0;
}

int main(int loaderparam, char** argv)
{
	//u8 *workbuf=(u8*)0x00690000;
	//u8 *slot1=(u8*)(0x00682000+0x1F000);
	Handle cptr;
	Handle nptr;
	//Handle cfgHandle;  using defines to avoid globals that require DATA and BSS section
	Handle nsHandle;
	Result res;
	int y=40;
	
	Handle* gspHandle=(Handle*)GSPGPU_SERVHANDLEADR;
	u32 *linear_buffer = (u32*)VRAM;
	_GSPGPU_SetLcdForceBlack(*gspHandle, 0); //our bb3 loader sets this to black, so we have to undo it to see anything

	// put framebuffers in linear mem so they're writable
	u32* top_framebuffer = linear_buffer;
	u32* low_framebuffer = (linear_buffer+0x46500);
	_GSPGPU_SetBufferSwap(*gspHandle, 0, (GSP_FramebufferInfo){0, (u32*)top_framebuffer, (u32*)top_framebuffer, 240 * 3, (1<<8)|(1<<6)|1, 0, 0});
	_GSPGPU_SetBufferSwap(*gspHandle, 1, (GSP_FramebufferInfo){0, (u32*)low_framebuffer, (u32*)low_framebuffer, 240 * 3, 1, 0, 0});
	
	 _initSrv(&cptr);
	 
	drawTitleScreen("");
	 
	res = _srv_getServiceHandle(&cptr, cfgHandle, "cfg:i");
	renderString("cfg:i ", 0, y);
	drawHex(res, 48, y); //cfg:i get result
	if(res){
		renderString("WHAT IS WRONG WITH THE ELF?", 0, y+10);
		renderString("Hold power to turn off :(", 0, y+20);
		while(1) svc_sleepThread(100*1000*1000);
	}
	
	_initSrv(&nptr);
	res = _srv_getServiceHandle(&nptr, &nsHandle, "ns:s");
	renderString("ns:s  ", 0, y+10);
	drawHex(res, 48, y+10); //ns:s get result
	
	y=120;
	centerString("Press...                     ", y+=10);
	centerString("L+R shoulder -> Inject haxx  ", y+=10);
	centerString("Dpad  Up + A -> Restore slots", y+=10);
	centerString("START        -> Exit         ", y+=10);
	
	check_slots();     //shows status of all 3 slots (haxx or user)
	
	while(1){
		svc_sleepThread(17*1000*1000);
		if(HID_PAD & PAD_START){
			drawTitleScreen("Exiting now");
			svc_sleepThread(0x40000000);
			break;
		}
		else if ((HID_PAD & PAD_L) && (HID_PAD & PAD_R)){
			inject_slots();
			drawTitleScreen("Slots injected!");
			svc_sleepThread(0x40000000);
			drawTitleScreen("Shutting down now...");
			svc_sleepThread(0x40000000);
			NS_RebootSystem(nsHandle, 0xE0000);
			while(1) svc_sleepThread(100*1000*1000);
		}
		else if ((HID_PAD & PAD_UP) && (HID_PAD & PAD_A)){
			restore_slots();
			drawTitleScreen("Slots restored!");
			svc_sleepThread(0x40000000);
			break;
		}
	}
 
	drawTitleScreen("Rebooting...");
	NS_RebootSystem(nsHandle, 0x160000);
	while(1) svc_sleepThread(100*1000*1000);
	return 0;
}