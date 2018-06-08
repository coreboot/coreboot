/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/

/**
 * @file
 *
 * Implementation of spinlocks.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @addtogroup hal
 * @{
 */

/**
 * Spinlocks
 */
typedef union
{
    uint64_t combined;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint32_t ticket;
        uint32_t serving;
#else
        uint32_t serving;
        uint32_t ticket;
#endif
    } s;
} bdk_spinlock_t;

/**
 * Initialize a spinlock
 *
 * @param lock   Lock to initialize
 */
static inline void bdk_spinlock_init(bdk_spinlock_t *lock)
{
    asm volatile ("str xzr, [%[b]]"
                  : "+m" (lock->combined)
                  : [b] "r" (&lock->combined)
                  : "memory");
}

/**
 * Releases lock
 *
 * @param lock   pointer to lock structure
 */
static inline void bdk_spinlock_unlock(bdk_spinlock_t *lock) __attribute__ ((always_inline));
static inline void bdk_spinlock_unlock(bdk_spinlock_t *lock)
{
    /* Implies a release */
    asm volatile ("stlr %w[v], [%[b]]"
                  : "+m" (lock->s.serving)
                  : [v] "r" (lock->s.serving + 1), [b] "r" (&lock->s.serving)
                  : "memory");
}

/**
 * Gets lock, spins until lock is taken
 *
 * @param lock   pointer to lock structure
 */
static inline void bdk_spinlock_lock(bdk_spinlock_t *lock) __attribute__ ((always_inline));
static inline void bdk_spinlock_lock(bdk_spinlock_t *lock)
{
    uint64_t combined;
    uint32_t ticket;
    uint32_t serving;

    asm volatile (
        "mov %x[serving], 1<<32                     \n"
        "ldadda %x[serving], %x[combined], [%[ptr]] \n"
        "and %x[serving], %x[combined], 0xffffffff  \n"
        "lsr %x[ticket], %x[combined], 32           \n"
        "cmp %x[ticket], %x[serving]                \n"
        "b.eq 1f                                    \n"
        "sevl                                       \n"
     "2: wfe                                        \n"
        "ldxr %w[serving], [%[ptr2]]                \n"
        "cmp %x[ticket], %x[serving]                \n"
        "b.ne 2b                                    \n"
     "1:                                            \n"
        : [serving] "=&r" (serving), [ticket] "=&r" (ticket), [combined] "=&r" (combined), "+m" (lock->combined)
        : [ptr] "r" (&lock->combined), [ptr2] "r" (&lock->s.serving)
        : "memory"
    );
}

/**
 * Trys to get the lock, failing if we can't get it immediately
 *
 * @param lock   pointer to lock structure
 */
static inline int bdk_spinlock_trylock(bdk_spinlock_t *lock) __attribute__ ((always_inline));
static inline int bdk_spinlock_trylock(bdk_spinlock_t *lock)
{
    uint64_t combined = *(volatile uint64_t *)&lock->combined;
    uint32_t ticket = combined >> 32;
    uint32_t serving = (uint32_t)combined;
    if (ticket != serving)
        return -1;
    uint64_t new_combined = combined + (1ull << 32);
    bool success = bdk_atomic_compare_and_store64(&lock->combined, combined, new_combined);
    return success ? 0 : -1;
}

/** @} */
