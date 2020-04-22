/*
  srv.c _ Service manager.
*/

#include "types.h"
#include "result.h"
#include "svc.h"
#include "srv.h"
#include "synchronization.h"
#include "srvpm.h"
#include "ipc.h"
#include "os.h"

static size_t strnlen8(const char *s)
{
    char buf[9] = {0};
    strncpy(buf, s, 8);
    return strlen(buf);
}

Result srvInit(Handle* srvHandle, const Handle* srvPmHandle)
{
    Result rc = 0;

    if(osGetFirmVersion() < SYSTEM_VERSION(2, 39, 4) && *srvPmHandle != 0)
        rc = svcDuplicateHandle(srvHandle, *srvPmHandle); // Prior to system version 7.0 srv:pm was a superset of srv:
    else
        rc = svcConnectToPort(srvHandle, "srv:");
    if (R_FAILED(rc)) goto end;

    rc = srvRegisterClient(srvHandle);
end:
    if (R_FAILED(rc)) srvExit(srvHandle);
    return rc;
}

void srvExit(Handle* srvHandle)
{
    if (*srvHandle != 0) svcCloseHandle(*srvHandle);
    *srvHandle = 0;
}

Result srvGetServiceHandle(const Handle* srvHandle, Handle* out, const char* name)
{
    /* Normal request to service manager. */
    return srvGetServiceHandleDirect(srvHandle, out, name);
}

Result srvRegisterClient(const Handle* srvHandle)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x1,0,2); // 0x10002
    cmdbuf[1] = IPC_Desc_CurProcessHandle();

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    return cmdbuf[1];
}

Result srvEnableNotification(const Handle* srvHandle, Handle* semaphoreOut)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x2,0,0);

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    if(semaphoreOut) *semaphoreOut = cmdbuf[3];

    return cmdbuf[1];
}

Result srvRegisterService(const Handle* srvHandle, Handle* out, const char* name, int maxSessions)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x3,4,0); // 0x30100
    strncpy((char*) &cmdbuf[1], name,8);
    cmdbuf[3] = strnlen8(name);
    cmdbuf[4] = maxSessions;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    if(out) *out = cmdbuf[3];

    return cmdbuf[1];
}

Result srvUnregisterService(const Handle* srvHandle, const char* name)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x4,3,0); // 0x400C0
    strncpy((char*) &cmdbuf[1], name,8);
    cmdbuf[3] = strnlen8(name);

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    return cmdbuf[1];
}

Result srvGetServiceHandleDirect(const Handle* srvHandle, Handle* out, const char* name)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x5,4,0); // 0x50100
    strncpy((char*) &cmdbuf[1], name,8);
    cmdbuf[3] = strnlen8(name);
    cmdbuf[4] = 0x0;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    if(out) *out = cmdbuf[3];

    return cmdbuf[1];
}

Result srvRegisterPort(const Handle* srvHandle, const char* name, Handle clientHandle)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x6,3,2); // 0x600C2
    strncpy((char*) &cmdbuf[1], name,8);
    cmdbuf[3] = strnlen8(name);
    cmdbuf[4] = IPC_Desc_SharedHandles(1);
    cmdbuf[5] = clientHandle;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    return cmdbuf[1];
}

Result srvUnregisterPort(const Handle* srvHandle, const char* name)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x7,3,0); // 0x700C0
    strncpy((char*) &cmdbuf[1], name,8);
    cmdbuf[3] = strnlen8(name);

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    return cmdbuf[1];
}

Result srvGetPort(const Handle* srvHandle, Handle* out, const char* name)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x8,4,0); // 0x80100
    strncpy((char*) &cmdbuf[1], name,8);
    cmdbuf[3] = strnlen8(name);
    cmdbuf[4] = 0x0;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    if(out) *out = cmdbuf[3];

    return cmdbuf[1];
}

Result srvSubscribe(const Handle* srvHandle, u32 notificationId)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x9,1,0); // 0x90040
    cmdbuf[1] = notificationId;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    return cmdbuf[1];
}

Result srvUnsubscribe(const Handle* srvHandle, u32 notificationId)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0xA,1,0); // 0xA0040
    cmdbuf[1] = notificationId;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    return cmdbuf[1];
}

Result srvReceiveNotification(const Handle* srvHandle, u32* notificationIdOut)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0xB,0,0); // 0xB0000

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    if(notificationIdOut) *notificationIdOut = cmdbuf[2];

    return cmdbuf[1];
}

Result srvPublishToSubscriber(const Handle* srvHandle, u32 notificationId, u32 flags)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0xC,2,0); // 0xC0080
    cmdbuf[1] = notificationId;
    cmdbuf[2] = flags;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    return cmdbuf[1];
}

Result srvPublishAndGetSubscriber(const Handle* srvHandle, u32* processIdCountOut, u32* processIdsOut, u32 notificationId)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0xD,1,0); // 0xD0040
    cmdbuf[1] = notificationId;

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    if(processIdCountOut) *processIdCountOut = cmdbuf[2];
    if(processIdsOut) memcpy(processIdsOut, &cmdbuf[3], cmdbuf[2] * sizeof(u32));

    return cmdbuf[1];
}

Result srvIsServiceRegistered(const Handle* srvHandle, bool* registeredOut, const char* name)
{
    Result rc = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0xE,3,0); // 0xE00C0
    strncpy((char*) &cmdbuf[1], name,8);
    cmdbuf[3] = strnlen8(name);

    if(R_FAILED(rc = svcSendSyncRequest(*srvHandle)))return rc;

    if(registeredOut) *registeredOut = cmdbuf[2] & 0xFF;

    return cmdbuf[1];
}
