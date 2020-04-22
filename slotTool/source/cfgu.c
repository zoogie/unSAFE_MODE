#include <stdlib.h>
#include <3ds/types.h>
#include <3ds/result.h>
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/synchronization.h>
#include <3ds/services/cfgu.h>
#include <3ds/ipc.h>

static Handle _cfguHandle;
static int _cfguRefCount;

Result _cfguInit(void)
{
	Result ret;

	if (AtomicPostIncrement(&_cfguRefCount)) return 0;

	// cfg:i has the most commands, then cfg:s, then cfg:u
	ret = srvGetServiceHandle(&_cfguHandle, "cfg:i");
	if(R_FAILED(ret)) ret = srvGetServiceHandle(&_cfguHandle, "cfg:s");
	if(R_FAILED(ret)) ret = srvGetServiceHandle(&_cfguHandle, "cfg:u");
	if(R_FAILED(ret)) AtomicDecrement(&_cfguRefCount);

	return ret;
}

void _cfguExit(void)
{
	if (AtomicDecrement(&_cfguRefCount)) return;
	svcCloseHandle(_cfguHandle);
}

Result _CFG_GetConfigInfoBlk4(u32 size, u32 blkID, u8* outData)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x401,2,2); // 0x4010082
	cmdbuf[1] = size;
	cmdbuf[2] = blkID;
	cmdbuf[3] = IPC_Desc_Buffer(size,IPC_BUFFER_W);
	cmdbuf[4] = (u32)outData;

	if(R_FAILED(ret = svcSendSyncRequest(_cfguHandle)))return ret;

	return (Result)cmdbuf[1];
}

Result _CFG_SetConfigInfoBlk4(u32 size, u32 blkID, u8* inData)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x402,2,2); // 0x4020082
	cmdbuf[1] = blkID;
	cmdbuf[2] = size;
	cmdbuf[3] = IPC_Desc_Buffer(size,IPC_BUFFER_R);
	cmdbuf[4] = (u32)inData;

	if(R_FAILED(ret = svcSendSyncRequest(_cfguHandle)))return ret;

	return (Result)cmdbuf[1];
}


Result _CFG_UpdateConfigSavegame(void)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x4030000; // changed from 0x8030000 to be compatible with both cfg:i and cfg:s

	if(R_FAILED(ret = svcSendSyncRequest(_cfguHandle)))return ret;

	return (Result)cmdbuf[1];
}

