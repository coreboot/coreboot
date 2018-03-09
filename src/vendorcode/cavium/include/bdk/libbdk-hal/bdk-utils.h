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
#include "libbdk-arch/bdk-csrs-lmc.h"
#include "libbdk-arch/bdk-csrs-rst.h"

/**
 * @file
 * Small utility functions and macros to ease programming.
 *
 * <hr>$Revision: 38306 $<hr>
 *
 * @addtogroup hal
 * @{
 */

/*
 * The macros bdk_likely and bdk_unlikely use the
 * __builtin_expect GCC operation to control branch
 * probabilities for a conditional. For example, an "if"
 * statement in the code that will almost always be
 * executed should be written as "if (bdk_likely(...))".
 * If the "else" section of an if statement is more
 * probable, use "if (bdk_unlikey(...))".
 */
#define bdk_likely(x)      __builtin_expect(!!(x), 1)
#define bdk_unlikely(x)    __builtin_expect(!!(x), 0)

#define BDK_DISPLAY_PASS       1    /* Control the display of the detail chip pass info */
#define BDK_CACHE_LINE_SIZE    (128)   // In bytes
#define BDK_CACHE_LINE_MASK    (BDK_CACHE_LINE_SIZE - 1)   // In bytes
#define BDK_CACHE_LINE_ALIGNED __attribute__ ((aligned (BDK_CACHE_LINE_SIZE)))

/**
 * Builds a bit mask given the required size in bits.
 *
 * @param bits   Number of bits in the mask
 * @return The mask
 */
static inline uint64_t bdk_build_mask(uint64_t bits)
{
    if (bits == 64)
        return -1;
    else
        return ~((~0x0ull) << bits);
}

/**
 * Extract bits out of a number
 *
 * @param input  Number to extract from
 * @param lsb    Starting bit, least significant (0-63)
 * @param width  Width in bits (1-64)
 *
 * @return Extracted number
 */
static inline uint64_t bdk_extract(uint64_t input, int lsb, int width)
{
    uint64_t result = input >> lsb;
    result &= bdk_build_mask(width);
    return result;
}

/**
 * Extract signed bits out of a number
 *
 * @param input  Number to extract from
 * @param lsb    Starting bit, least significant (0-63)
 * @param width  Width in bits (1-64)
 *
 * @return Extracted number
 */
static inline int64_t bdk_extracts(uint64_t input, int lsb, int width)
{
    int64_t result = input >> lsb;
    result <<= 64 - width;
    result >>= 64 - width;
    return result;
}

/**
 * Extract a signed magnatude value. Signed magnatude is a value where the MSB
 * is treated as a sign bit, not like the normal twos compliment
 *
 * @param v      Value to extract from
 * @param lsb    LSB of number
 * @param msb    MSB, which is the signed bit
 *
 * @return Extracted number
 */
static inline int64_t bdk_extract_smag(uint64_t v, int lsb, int msb) __attribute__((always_inline));
static inline int64_t bdk_extract_smag(uint64_t v, int lsb, int msb)
{
    int64_t r = bdk_extract(v, lsb, msb - lsb);
    if (v & (1ull << msb))
        r = -r;
    return r;
}

/**
 * Insert bits into a number
 *
 * @param original Original data, before insert
 * @param input    Data to insert
 * @param lsb    Starting bit, least significant (0-63)
 * @param width  Width in bits (1-64)
 *
 * @return Number with inserted bits
 */
static inline uint64_t bdk_insert(uint64_t original, uint64_t input, int lsb, int width) __attribute__((always_inline));
static inline uint64_t bdk_insert(uint64_t original, uint64_t input, int lsb, int width)
{
    uint64_t mask = bdk_build_mask(width);
    uint64_t result = original & ~(mask << lsb);
    result |= (input & mask) << lsb;
    return result;
}

/**
 * Return the number of cores available in the chip
 *
 * @return
 */
static inline int bdk_get_num_cores(bdk_node_t node)
{
    uint64_t available = BDK_CSR_READ(node, BDK_RST_PP_AVAILABLE);
    return bdk_dpop(available);
}


/**
 * Return true if DRAM has been configured
 *
 * @return Boolean
 */
static inline int __bdk_is_dram_enabled(bdk_node_t node) __attribute__((always_inline));
static inline int __bdk_is_dram_enabled(bdk_node_t node)
{
    BDK_CSR_INIT(lmcx_ddr_pll_ctl, node, BDK_LMCX_DDR_PLL_CTL(0));
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return lmcx_ddr_pll_ctl.cn83xx.reset_n;
    else
        return !lmcx_ddr_pll_ctl.cn9.pll_reset;
}

/**
 * Zero a block of memory
 *
 * @param start
 * @param length
 */
static inline void bdk_zero_memory(void *start, uint64_t length) __attribute__((always_inline));
static inline void bdk_zero_memory(void *start, uint64_t length)
{
    if (((long)start & BDK_CACHE_LINE_MASK) || (length & BDK_CACHE_LINE_MASK))
    {
        /* Use slwo memset for unaligned memory */
        memset(start, 0, length);
    }
    else
    {
        void *end = start + length;
        while (start<end)
        {
            asm volatile ("dc zva,%0" : : "r"(start));
            start += BDK_CACHE_LINE_SIZE;
        }
    }
}

/** @} */
