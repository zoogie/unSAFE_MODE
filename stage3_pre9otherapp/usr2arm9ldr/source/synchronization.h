/**
 * @file synchronization.h
 * @brief Provides synchronization locks.
 */
#pragma once
#include "svc.h"

/// Performs a Data Synchronization Barrier operation.
static inline void __dsb(void)
{
    __asm__ __volatile__("mcr p15, 0, %[val], c7, c10, 4" :: [val] "r" (0) : "memory");
}

/// Performs a clrex operation.
static inline void __clrex(void)
{
    __asm__ __volatile__("clrex" ::: "memory");
}

/**
 * @brief Performs a ldrex operation.
 * @param addr Address to perform the operation on.
 * @return The resulting value.
 */
static inline s32 __ldrex(s32* addr)
{
    s32 val;
    __asm__ __volatile__("ldrex %[val], %[addr]" : [val] "=r" (val) : [addr] "Q" (*addr));
    return val;
}

/**
 * @brief Performs a strex operation.
 * @param addr Address to perform the operation on.
 * @param val Value to store.
 * @return Whether the operation was successful.
 */
static inline bool __strex(s32* addr, s32 val)
{
    bool res;
    __asm__ __volatile__("strex %[res], %[val], %[addr]" : [res] "=&r" (res) : [val] "r" (val), [addr] "Q" (*addr));
    return res;
}

/**
 * @brief Performs a ldrexh operation.
 * @param addr Address to perform the operation on.
 * @return The resulting value.
 */
static inline u16 __ldrexh(u16* addr)
{
    u16 val;
    __asm__ __volatile__("ldrexh %[val], %[addr]" : [val] "=r" (val) : [addr] "Q" (*addr));
    return val;
}

/**
 * @brief Performs a strexh operation.
 * @param addr Address to perform the operation on.
 * @param val Value to store.
 * @return Whether the operation was successful.
 */
static inline bool __strexh(u16* addr, u16 val)
{
    bool res;
    __asm__ __volatile__("strexh %[res], %[val], %[addr]" : [res] "=&r" (res) : [val] "r" (val), [addr] "Q" (*addr));
    return res;
}

/**
 * @brief Performs a ldrexb operation.
 * @param addr Address to perform the operation on.
 * @return The resulting value.
 */
static inline u8 __ldrexb(u8* addr)
{
    u8 val;
    __asm__ __volatile__("ldrexb %[val], %[addr]" : [val] "=r" (val) : [addr] "Q" (*addr));
    return val;
}

/**
 * @brief Performs a strexb operation.
 * @param addr Address to perform the operation on.
 * @param val Value to store.
 * @return Whether the operation was successful.
 */
static inline bool __strexb(u8* addr, u8 val)
{
    bool res;
    __asm__ __volatile__("strexb %[res], %[val], %[addr]" : [res] "=&r" (res) : [val] "r" (val), [addr] "Q" (*addr));
    return res;
}

/// Performs an atomic pre-increment operation.
#define AtomicIncrement(ptr) __atomic_add_fetch((u32*)(ptr), 1, __ATOMIC_SEQ_CST)
/// Performs an atomic pre-decrement operation.
#define AtomicDecrement(ptr) __atomic_sub_fetch((u32*)(ptr), 1, __ATOMIC_SEQ_CST)
/// Performs an atomic post-increment operation.
#define AtomicPostIncrement(ptr) __atomic_fetch_add((u32*)(ptr), 1, __ATOMIC_SEQ_CST)
/// Performs an atomic post-decrement operation.
#define AtomicPostDecrement(ptr) __atomic_fetch_sub((u32*)(ptr), 1, __ATOMIC_SEQ_CST)
/// Performs an atomic swap operation.
#define AtomicSwap(ptr, value) __atomic_exchange_n((u32*)(ptr), (value), __ATOMIC_SEQ_CST)
