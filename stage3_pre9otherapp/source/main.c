#include <string.h>
#include "gspgpu.h"
#include "text.h"

#include "exploits/rsa_exploit.h"
#include "exploits/memchunkhax.h"
#include "exploits/brahma.h"

#include "libctru/types.h"
#include "libctru/svc.h"
#include "libctru/srv.h"

#define PA_PTR(addr) (void *)((u32)(addr) | 1 << 31)

const char* const aptServiceNames[] = {"APT:U", "APT:A", "APT:S"};

#define aptSessionInit() \
	int aptIndex; \
	for(aptIndex = 0; aptIndex < 3; aptIndex++)	if(!srvGetServiceHandle(&aptuHandle, (char*)aptServiceNames[aptIndex]))break;\
	svcCloseHandle(aptuHandle);\

#define aptOpenSession() \
	svcWaitSynchronization(aptLockHandle, U64_MAX);\
	srvGetServiceHandle(&aptuHandle, (char*)aptServiceNames[aptIndex]);\

#define aptCloseSession()\
	svcCloseHandle(aptuHandle);\
	svcReleaseMutex(aptLockHandle);\
	
#define HID_PAD (*(vu32*)0x1000001C)


Result APT_GetLockHandle(Handle* handle, u16 flags, Handle* lockHandle)
{
	u32* cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = 0x10040; //request header code
	cmdbuf[1] = flags;
	
	Result ret=0;
	if((ret=svcSendSyncRequest(*handle)))return ret;
	
	if(lockHandle)*lockHandle=cmdbuf[5];
	
	return cmdbuf[1];
}

Result APT_CheckNew3ds(Handle *handle, bool *out)
{
	u32 *cmdbuf = getThreadCommandBuffer(); 
	cmdbuf[0]=0x1020000;

	Result ret = svcSendSyncRequest(*handle);
	if (ret == 0)
		*out = cmdbuf[2] & 0xFF;

	return ret;
}

const u8 hexTable[]=
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

void hex2str(char* out, u32 val)
{
	int i;
	for(i=0;i<8;i++){out[7-i]=hexTable[val&0xf];val>>=4;}
	out[8]=0x00;
}

void renderString(char* str, int x, int y)
{
	u8 *ptr = GSP_GetScreenFBADR(0);
	if(ptr==NULL)return;
	drawString(ptr,str,x,y);
	GSP_FlushDCache((u32*)ptr, 240*400*3);
}

void centerString(char* str, int y)
{
	renderString(str, 200-(strlen(str)*4), y);
}

void drawHex(u32 val, int x, int y)
{
	char str[9];

	hex2str(str,val);
	renderString(str,x,y);
}

void doGspwn(u32* src, u32* dst, u32 size)
{
	Result (*gxcmd4)(u32 *src, u32 *dst, u32 size, u16 width0, u16 height0, u16 width1, u16 height1, u32 flags);
	u32 *paramblk = (u32*)*((u32*)0xFFFFFFC);
	gxcmd4=(void*)paramblk[0x1c>>2];
	gxcmd4(src, dst, size, 0, 0, 0, 0, 0x8);
}

void clearScreen(u8 shade)
{
	u8 *ptr = GSP_GetScreenFBADR(0);
	if(ptr==NULL)return;
	memset(ptr, shade, 240*400*3);
	GSP_FlushDCache((u32*)ptr, 240*400*3);
}

void clearScreenB(u8 shade)
{
	u8 *ptr = GSP_GetScreenFBADR(2);
	if(ptr==NULL)return;
	memset(ptr, shade, 240*320*3);
	GSP_FlushDCache((u32*)ptr, 240*320*3);
}

void errorScreen(char* str, u32* dv, u8 n)
{
	clearScreen(0x00);
	renderString("FATAL ERROR",0,0);
	renderString(str,0,10);
	if(dv && n)
	{
		int i;
		for(i=0;i<n;i++)drawHex(dv[i], 8, 50+i*10);
	}
	while(1);
}

void drawTitleScreen(char* str)
{
	clearScreen(0x00);
	centerString("OTHERAPP KARTIK",0);
	centerString("TRY BUILD",10);
	centerString("smealum.github.io/ninjhax2/",20);
	centerString(str, 30);
}

bool g_is_new3ds;
char* test_result;

s32 set_test_result(void)
{
   __asm__ volatile("cpsid aif \n\t");
   test_result = "SUCCESS !!!";
   return 0;
}

extern u8 *g_ext_arm9_buf;
extern u64 g_ext_arm9_size;

Result escalateServicePrivileges(Handle *wantedServiceHandle, const char *wantedServiceName);


Result _NS_RebootSystem(Handle *nsHandle)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] =  0x160000; // 0x160000
	
	ret = svcSendSyncRequest(*nsHandle);
	if(ret)return ret;

	return (Result)cmdbuf[1];
}

Result _NS_RebootToTitle(u8 mediatype, u64 titleid, Handle *nsHandle)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x100180; // 0x100180
	cmdbuf[1] = 0x0;
	cmdbuf[2] = titleid & 0xffffffff;
	cmdbuf[3] = (titleid >> 32) & 0xffffffff;
	cmdbuf[4] = mediatype;
	cmdbuf[5] = 0x0; // reserved
	cmdbuf[6] = 0x0;

	ret = svcSendSyncRequest(*nsHandle);
	if(ret)return ret;

	return (Result)cmdbuf[1];
}

const u8 __attribute__ ((section (".rodata"))) access_bin[] =
	{
		0x41, 0x50, 0x54, 0x3A, 0x55, 0x00, 0x00, 0x00, 0x79, 0x32, 0x72, 0x3A, 0x75, 0x00, 0x00, 0x00, 
		0x67, 0x73, 0x70, 0x3A, 0x3A, 0x47, 0x70, 0x75, 0x6E, 0x64, 0x6D, 0x3A, 0x75, 0x00, 0x00, 0x00, 
		0x66, 0x73, 0x3A, 0x55, 0x53, 0x45, 0x52, 0x00, 0x68, 0x69, 0x64, 0x3A, 0x55, 0x53, 0x45, 0x52, 
		0x64, 0x73, 0x70, 0x3A, 0x3A, 0x44, 0x53, 0x50, 0x63, 0x66, 0x67, 0x3A, 0x75, 0x00, 0x00, 0x00, 
		0x66, 0x73, 0x3A, 0x52, 0x45, 0x47, 0x00, 0x00, 0x70, 0x73, 0x3A, 0x70, 0x73, 0x00, 0x00, 0x00, 
		0x6E, 0x73, 0x3A, 0x73, 0x00, 0x00, 0x00, 0x00, 0x61, 0x6D, 0x3A, 0x6E, 0x65, 0x74, 0x00, 0x00, 
	};

Result srvRegisterProcess(Handle *handle, u32 procid, u32 count, void *serviceaccesscontrol)
{
	Result rc = 0;
	
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x04030082; // <7.x
	cmdbuf[1] = procid;
	cmdbuf[2] = count;
	cmdbuf[3] = (count << 16) | 2;
	cmdbuf[4] = (u32)serviceaccesscontrol;
	
	if((rc = svcSendSyncRequest(*handle))) return rc;
		
	return cmdbuf[1];
}

Result srvUnregisterProcess(Handle *handle, u32 procid)
{
	Result rc = 0;
	
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x04040040; // <7.x
	cmdbuf[1] = procid;
	
	if((rc = svcSendSyncRequest(*handle))) return rc;
		
	return cmdbuf[1];
}

Result _srv_getServiceHandle(Handle* handleptr, Handle* out, char* server)
{
	u8 l=strlen(server);
	if(!out || !server || l>8)return -1;

	u32* cmdbuf=getThreadCommandBuffer();

	cmdbuf[0]=0x50100; //request header code
	strcpy((char*)&cmdbuf[1], server);
	cmdbuf[3]=l;
	cmdbuf[4]=0x0;

	Result ret=0;
	if((ret=svcSendSyncRequest(*handleptr)))return ret;

	*out=cmdbuf[3];

	return cmdbuf[1];
}


u32 hook1_pa = 0x0;
u32 hook2_pa = 0x0;
u32 return_pa = 0x0;
u32 kernel_set_state = 0x0;
u32 fb[3];

s32 get_offsets(void)
{
	__asm__ volatile("cpsid aif \n\t");
	hook1_pa = 0xFFFFFFFF;
	hook2_pa = 0xFFFFFFFF;
	return_pa = 0xFFFFFFFF;
	kernel_set_state = 0xFFFFFFFF;
	
	memcpy((u32*)0xf3000000, (u32*)0xeff80000, 0x80000);
	//*
	for(u32 i = 0xeFF80000; i < 0xf0000000-0x20; i+=4)
	{
		u32 *val = (u32*)i;
		//printf("val: %lX\n", *val);
		if(val[0] == 0xE28F0010) { hook2_pa = i; }
		if(val[0] == 0xE3A00080 && val[1] == 0xE5810000 && val[3] == 0xE5901000){ hook1_pa = i - 16; return_pa = i - 8; }
		if(val[0] == 0xE1A00000 && val[2] == 0xE24DD014){ kernel_set_state = i + 8; }
	}
	//*/
	return 0;
}

s32 set_framebuffers(){  //straight from brahma
	u32 *save = (u32*)(0xF0000000 + 0x3FFFE00);
	save[0] = fb[0]+0x0C000000;
   	save[1] = fb[1]+0x0C000000;
	save[2] = fb[2]+0x0C000000;
}

int main(u32 loaderparam, char** argv)
{
	
	//Handle *nsHandle=(u32*)0x00260120;
	
	Handle srvHandle;
	Result ret;
	
	u32 *paramblk = (u32*)loaderparam;
	Handle* gspHandle=(Handle*)paramblk[0x58>>2];
	u32* linear_buffer = (u32*)((((u32)paramblk) + 0x1000) & ~0xfff);
	
	// put framebuffers in linear mem so they're writable
	u8* top_framebuffer = &linear_buffer[0x00100000/4];
	u8* low_framebuffer = &top_framebuffer[0x00046500];
	
	GSP_SetBufferSwap(*gspHandle, 0, (GSPGPU_FramebufferInfo){0, (u32*)top_framebuffer, (u32*)top_framebuffer, 240 * 3, (1<<8)|(1<<6)|1, 0, 0});
	GSP_SetBufferSwap(*gspHandle, 1, (GSPGPU_FramebufferInfo){0, (u32*)low_framebuffer, (u32*)low_framebuffer, 240 * 3, 1, 0, 0});
	
	drawTitleScreen("SAFEHAX 3.0");
	
	u32 fversion = (*(vu32*)0x1FF80000) & ~0xFF;
	renderString("Firm version: ", 8, 50);
	drawHex(fversion, 170, 50);
	bool isN3ds=false;
	(*(u32*)0x1FF80030 >= 6) ? renderString("NEW3DS", 250, 50) : renderString("OLD3DS", 250, 50);
	svcSleepThread(500*1000*1000);
	
	if(fversion == 0x03200000) // (old3ds safefirm) keeping it simple, we're only dealing with two possible firms with this version of safehax
	{
		
		renderString("SrvInit()", 8, 60);
		srvInit(&srvHandle, NULL);
		
		renderString("Trying memchunkhax", 8, 60);
		do_memchunkhax1();
		
		renderString("Trying backdoor", 8, 60);
		svc_7b((backdoor_fn)k_enable_all_svcs, isN3ds);
		renderString("Unblocked svcs    ", 8, 60);
		svcSleepThread(500*1000*1000);
	
		test_result ="FAILED !!!";
		svcBackdoor(set_test_result);
		renderString(test_result, 8, 70);
	
		renderString("Unblocking access to all services", 8, 80);
		unlock_services(isN3ds, &srvHandle);
		
		Handle ps_handle;
		renderString("Grabbing ps:ps", 8, 90);
		ret = srvGetServiceHandle(&ps_handle, "ps:ps");
		drawHex(ret, 8, 100);
		
		
		//svcBackdoor(get_offsets);
		/*
		while(1){
			svcSleepThread(0x10000000);
			drawHex(hook1_pa, 8, 90); 		//dffe6e6c   66e64
			drawHex(hook2_pa, 8, 100);		//dfff4994
			drawHex(return_pa, 8, 110);		//dffe6e74
			drawHex(kernel_set_state, 8, 120);	//dff95204
		};
		//*/
		
		renderString("Set framebuffers", 8, 110);
		fb[0]=0; fb[1]=0; fb[2]=0;
		fb[0] = GSP_GetScreenFBADR(0);
		fb[1] = GSP_GetScreenFBADR(1);
		fb[2] = GSP_GetScreenFBADR(2);
		svcBackdoor(set_framebuffers);
		
		drawHex(fb[0], 8, 120);
		drawHex(fb[1], 8, 130);
		drawHex(fb[2], 8, 140);
		
		svcSleepThread(500*1000*1000);
		
		renderString("PS_VerifyRsaSha256", 8, 150);
		ret = PS_VerifyRsaSha256_Exploit(&ps_handle, (u32*)0x00520000); //reverting back to usr2arm9ldr, it works great from here on out
		drawHex(ret, 170, 160);
		

	}
	else if(fversion==0x032D0300) //(new3ds safefirm) this firm is based on native firm 8.1, so we'll use memchunkhax + firmlaunchhax on it
	{		 	      //the only significant change besides hook offsets is kernelsetstate needs 3 instead of 2 for the third argumement to launch safefirm instead of nativefirm
		
		isN3ds=true;     //we can get away with hardcoding this I think
		
		renderString("SrvInit()", 8, 60);
		srvInit(&srvHandle, NULL);
		
		renderString("Trying memchunkhax", 8, 60);
		do_memchunkhax1();
		
		renderString("Trying backdoor", 8, 60);
		svc_7b((backdoor_fn)k_enable_all_svcs, isN3ds);
		renderString("Unblocked svcs    ", 8, 60);
		svcSleepThread(500*1000*1000);
	
		test_result ="FAILED !!!";
		svcBackdoor(set_test_result);
		renderString(test_result, 8, 70);
	
		renderString("Unblocking access to all services", 8, 80);
		unlock_services(isN3ds, &srvHandle);
		
		/*
		svcBackdoor(get_offsets);
		//_NS_RebootSystem(nsHandle);
		ret = _NS_RebootToTitle(0, 0x0004001000021F00LL, nsHandle);
		while(1){
			svcSleepThread(0x10000000);
			drawHex(hook1_pa, 8, 90); 		//dffe6e6c   66e64
			drawHex(hook2_pa, 8, 100);		//dfff4994
			drawHex(return_pa, 8, 110);		//dffe6e74
			drawHex(kernel_set_state, 8, 120);	//dff95204
		};
		//*/
		
		renderString("Initing brahma", 8, 90);
		Result ret = brahma_init();
		drawHex(ret, 8, 100);
	
		renderString("Load /usm.bin", 8, 110);
		ret = load_arm9_payload_offset ("/usm.bin", 0x10000, 0);
		drawHex(ret, 8, 120);

		renderString("Firmlaunch hax", 8, 130);
		ret = firm_reboot(isN3ds);
		drawHex((u32)ret, 8, 140);
		
		//ret = pmInit();
		//drawHex((u32)ret, 8, 120);
		
		//ret = PM_LaunchFIRMSetParams(3, 0, NULL);
		//drawHex((u32)ret, 16, 130);
	}
	else{
		renderString("DON'T DO STUPID", 8, 80);  //https://twitter.com/VietnamCrystal/status/1231657992279359488
	}
	
	while(1)svcSleepThread(0xFFFFFFFF); //sleep long enough for memory to be written
	//drawTitleScreen("\n   The homemenu ropbin is ready.");

	
	return 0;
}