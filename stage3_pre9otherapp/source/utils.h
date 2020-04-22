#pragma once

u32 svcConvertVAToPA(u32 addr, int write_check);
void InvalidateEntireInstructionCache (void);
void CleanEntireDataCache (void);
void dsb(void);
void DisableInterrupts (void);
void EnableInterrupts (void);
void InvalidateEntireDataCache (void);
