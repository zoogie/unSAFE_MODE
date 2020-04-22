#include "types.h"
#include "result.h"
#include "svc.h"
#include "srv.h"
#include "synchronization.h"
#include "srvpm.h"
#include "ipc.h"
#include "os.h"

#define IS_PRE_7X (osGetFirmVersion() < SYSTEM_VERSION(2, 39, 4))

Result srvPmInit(Handle* srvPmHandle, Handle* srvHandle)
{
    Result res = 0;

    if (!IS_PRE_7X) res = srvInit(srvHandle, srvPmHandle);
    if (R_FAILED(res)) return res;

    if (!IS_PRE_7X)
        res = srvGetServiceHandleDirect(srvHandle, srvPmHandle, "srv:pm");
    else
    {
        res = svcConnectToPort(srvPmHandle, "srv:pm");
        if (R_SUCCEEDED(res)) res = srvInit(srvHandle, srvPmHandle);
    }

    if (R_FAILED(res)) srvPmExit(srvPmHandle, srvHandle);
    return res;
}

void srvPmExit(Handle* srvPmHandle, Handle* srvHandle)
{
    if (*srvHandle) srvExit(srvHandle);
    svcCloseHandle(*srvPmHandle);
    *srvPmHandle = 0;
}

static Result srvPmSendCommand(const Handle* srvPmHandle, u32* cmdbuf)
{
    Result rc = 0;
    if (IS_PRE_7X) cmdbuf[0] |= 0x04000000;
    rc = svcSendSyncRequest(*srvPmHandle);
    if (R_SUCCEEDED(rc)) rc = cmdbuf[1];

    return rc;
}

Result SRVPM_PublishToProcess(const Handle* srvPmHandle, u32 notificationId, Handle process)
{
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x1,1,2); // 0x10042
    cmdbuf[1] = notificationId;
    cmdbuf[2] = IPC_Desc_SharedHandles(1);
    cmdbuf[3] = process;

    return srvPmSendCommand(srvPmHandle, cmdbuf);
}

Result SRVPM_PublishToAll(const Handle* srvPmHandle, u32 notificationId)
{
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x2,1,0); // 0x20040
    cmdbuf[1] = notificationId;

    return srvPmSendCommand(srvPmHandle, cmdbuf);
}

Result SRVPM_RegisterProcess(const Handle* srvPmHandle, u32 pid, u32 count, const char (*serviceAccessControlList)[8])
{
    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x3,2,2); // 0x30082
    cmdbuf[1] = pid;
    cmdbuf[2] = count*2;
    cmdbuf[3] = IPC_Desc_StaticBuffer(count*8,0);
    cmdbuf[4] = (u32)serviceAccessControlList;

    return srvPmSendCommand(srvPmHandle, cmdbuf);
}

Result SRVPM_UnregisterProcess(const Handle* srvPmHandle, u32 pid)
{
    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x4,1,0); // 0x40040
    cmdbuf[1] = pid;

    return srvPmSendCommand(srvPmHandle, cmdbuf);
}

