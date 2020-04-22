/**
 * @file os.h
 * @brief OS related stuff.
 */
#pragma once
#include "svc.h"

/// Packs a system version from its components.
#define SYSTEM_VERSION(major, minor, revision) \
    (((major)<<24)|((minor)<<16)|((revision)<<8))

/// Retrieves the major version from a packed system version.
#define GET_VERSION_MAJOR(version)    ((version) >>24)

/// Retrieves the minor version from a packed system version.
#define GET_VERSION_MINOR(version)    (((version)>>16)&0xFF)

/// Retrieves the revision version from a packed system version.
#define GET_VERSION_REVISION(version) (((version)>> 8)&0xFF)

/// Memory regions.
typedef enum
{
    MEMREGION_ALL = 0,         ///< All regions.
    MEMREGION_APPLICATION = 1, ///< APPLICATION memory.
    MEMREGION_SYSTEM = 2,      ///< SYSTEM memory.
    MEMREGION_BASE = 3,        ///< BASE memory.
} MemRegion;

/**
 * @brief Gets the system's FIRM version.
 * @return The system's FIRM version.
 *
 * This can be used to compare system versions easily with @ref SYSTEM_VERSION.
 */
static inline u32 osGetFirmVersion(void)
{
    return (*(vu32*)0x1FF80060) & ~0xFF;
}

/**
 * @brief Gets the system's kernel version.
 * @return The system's kernel version.
 *
 * This can be used to compare system versions easily with @ref SYSTEM_VERSION.
 *
 * @code
 * if(osGetKernelVersion() > SYSTEM_VERSION(2,46,0)) printf("You are running 9.0 or higher\n");
 * @endcode
 */
static inline u32 osGetKernelVersion(void)
{
    return (*(vu32*)0x1FF80000) & ~0xFF;
}

/**
 * @brief Gets the size of the specified memory region.
 * @param region Memory region to check.
 * @return The size of the memory region, in bytes.
 */
static inline u32 osGetMemRegionSize(MemRegion region)
{
    if(region == MEMREGION_ALL) {
        return osGetMemRegionSize(MEMREGION_APPLICATION) + osGetMemRegionSize(MEMREGION_SYSTEM) + osGetMemRegionSize(MEMREGION_BASE);
    } else {
        return *(vu32*) (0x1FF80040 + (region - 1) * 0x4);
    }
}

/**
 * @brief Gets the number of used bytes within the specified memory region.
 * @param region Memory region to check.
 * @return The number of used bytes of memory.
 */
static inline s64 osGetMemRegionUsed(MemRegion region)
{
    s64 mem_used;
    svcGetSystemInfo(&mem_used, 0, region);
    return mem_used;
}

/**
 * @brief Gets the number of free bytes within the specified memory region.
 * @param region Memory region to check.
 * @return The number of free bytes of memory.
 */
static inline s64 osGetMemRegionFree(MemRegion region)
{
    return (s64) osGetMemRegionSize(region) - osGetMemRegionUsed(region);
}

/**
 * @brief Gets the current Wifi signal strength.
 * @return The current Wifi signal strength.
 *
 * Valid values are 0-3:
 * - 0 means the singal strength is terrible or the 3DS is disconnected from
 *   all networks.
 * - 1 means the signal strength is bad.
 * - 2 means the signal strength is decent.
 * - 3 means the signal strength is good.
 *
 * Values outside the range of 0-3 should never be returned.
 *
 * These values correspond with the number of wifi bars displayed by Home Menu.
 */
static inline u8 osGetWifiStrength(void)
{
    return *(vu8*)0x1FF81066;
}

/**
 * @brief Gets the state of the 3D slider.
 * @return The state of the 3D slider (0.0~1.0)
 */
static inline float osGet3DSliderState(void)
{
    return *(volatile float*)0x1FF81080;
}
