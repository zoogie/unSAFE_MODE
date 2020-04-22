/**
 * @file cfgu.h
 * @brief CFGU (Configuration) Service
 */
#pragma once

/// Initializes CFGU.
Result _cfguInit(void);

/// Exits CFGU.
void _cfguExit(void);

/**
 * @brief Gets a config info block with flags = 4.
 * @param size Size of the data to retrieve.
 * @param blkID ID of the block to retrieve.
 * @param outData Pointer to write the block data to.
 */
Result _CFG_GetConfigInfoBlk4(u32 size, u32 blkID, u8* outData);

/**
 * @brief Sets a config info block with flags = 4.
 * @param size Size of the data to retrieve.
 * @param blkID ID of the block to retrieve.
 * @param inData Pointer to block data to write.
 */
Result _CFG_SetConfigInfoBlk4(u32 size, u32 blkID, u8* inData);

/**
 * @brief Writes the CFG buffer in memory to the savegame in NAND.
 */
Result _CFG_UpdateConfigSavegame(void);


