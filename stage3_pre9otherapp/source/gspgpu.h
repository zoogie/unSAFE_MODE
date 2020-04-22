/**
 * @file gspgpu.h
 * @brief GSPGPU service.
 */
#pragma once

#define GSPGPU_REBASE_REG(r) ((r)-0x1EB00000)

#include "libctru/gsp.h"

Result GSP_ImportDisplayCaptureInfo(Handle* handle, GSPGPU_CaptureInfo *captureinfo);
u8 *GSP_GetScreenFBADR(u8 screen);
Result GSP_FlushDCache(u32* addr, u32 size);
Result GSP_WriteHWRegs(u32 regAddr, u32 *data, u8 size);
Result GSP_ReadHWRegs(u32 regAddr, u32 *data, u8 size);
Result GSP_ReleaseRight(Handle handle);
Result GSP_SetBufferSwap(Handle handle, u32 screenid, GSPGPU_FramebufferInfo framebufinfo);
Result GSP_InvalidateDataCache(void* adr, u32 size);