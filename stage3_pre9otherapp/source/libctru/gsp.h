/**
 * @file gspgpu.h
 * @brief GSPGPU service.
 */
#pragma once

#define GSPGPU_REBASE_REG(r) ((r)-0x1EB00000)
#include "types.h"

/// Framebuffer information.
typedef struct
{
	u32 active_framebuf;        ///< Active framebuffer. (0 = first, 1 = second)
	u32 *framebuf0_vaddr;       ///< Framebuffer virtual address, for the main screen this is the 3D left framebuffer.
	u32 *framebuf1_vaddr;       ///< For the main screen: 3D right framebuffer address.
	u32 framebuf_widthbytesize; ///< Value for 0x1EF00X90, controls framebuffer width.
	u32 format;                 ///< Framebuffer format, this u16 is written to the low u16 for LCD register 0x1EF00X70.
	u32 framebuf_dispselect;    ///< Value for 0x1EF00X78, controls which framebuffer is displayed.
	u32 unk;                    ///< Unknown.
} GSPGPU_FramebufferInfo;

/// Framebuffer format.
typedef enum
{
	GSP_RGBA8_OES=0,   ///< RGBA8. (4 bytes)
	GSP_BGR8_OES=1,    ///< BGR8. (3 bytes)
	GSP_RGB565_OES=2,  ///< RGB565. (2 bytes)
	GSP_RGB5_A1_OES=3, ///< RGB5A1. (2 bytes)
	GSP_RGBA4_OES=4    ///< RGBA4. (2 bytes)
} GSPGPU_FramebufferFormats;

/// Capture info entry.
typedef struct
{
	u32 *framebuf0_vaddr;       ///< Left framebuffer.
	u32 *framebuf1_vaddr;       ///< Right framebuffer.
	u32 format;                 ///< Framebuffer format.
	u32 framebuf_widthbytesize; ///< Framebuffer pitch.
} GSPGPU_CaptureInfoEntry;

/// Capture info.
typedef struct
{
	GSPGPU_CaptureInfoEntry screencapture[2]; ///< Capture info entries, one for each screen.
} GSPGPU_CaptureInfo;

/// GSPGPU events.
typedef enum
{
	GSPGPU_EVENT_PSC0 = 0, ///< Memory fill completed.
	GSPGPU_EVENT_PSC1,     ///< TODO
	GSPGPU_EVENT_VBlank0,  ///< TODO
	GSPGPU_EVENT_VBlank1,  ///< TODO
	GSPGPU_EVENT_PPF,      ///< Display transfer finished.
	GSPGPU_EVENT_P3D,      ///< Command list processing finished.
	GSPGPU_EVENT_DMA,      ///< TODO

	GSPGPU_EVENT_MAX,      ///< Used to know how many events there are.
} GSPGPU_Event;
