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
 * Interface to Core, IO and DDR Clock.
 *
 * <hr>$Revision: 45089 $<hr>
 *
 * @addtogroup hal
 * @{
 */

#define BDK_GTI_RATE 100000000ull

/**
 * Enumeration of different Clocks.
 */
typedef enum{
    BDK_CLOCK_TIME,     /**< Clock for telling time with fast access. Uses GTI in core */
    BDK_CLOCK_MAIN_REF, /**< Main reference clock */
    BDK_CLOCK_RCLK,     /**< Clock used by cores, coherent bus and L2 cache. */
    BDK_CLOCK_SCLK,     /**< Clock used by IO blocks. */
} bdk_clock_t;

/**
 * Called in __bdk_init to setup the global timer
 */
extern void bdk_clock_setup();

/**
 * Get cycle count based on the clock type.
 *
 * @param clock - Enumeration of the clock type.
 * @return      - Get the number of cycles executed so far.
 */
static inline uint64_t bdk_clock_get_count(bdk_clock_t clock) __attribute__ ((always_inline));
static inline uint64_t bdk_clock_get_count(bdk_clock_t clock)
{
    extern uint64_t __bdk_clock_get_count_slow(bdk_clock_t clock);
    if (clock == BDK_CLOCK_TIME)
    {
        uint64_t clk;
        BDK_MRS(CNTPCT_EL0, clk);
        return clk;
    }
    else
        return __bdk_clock_get_count_slow(clock);
}

/**
 * Get clock rate based on the clock type.
 *
 * @param node    Node to use in a Numa setup. Can be an exact ID or a special value.
 * @param clock - Enumeration of the clock type.
 * @return      - return the clock rate.
 */
static inline uint64_t bdk_clock_get_rate(bdk_node_t node, bdk_clock_t clock) __attribute__ ((always_inline, pure));
static inline uint64_t bdk_clock_get_rate(bdk_node_t node, bdk_clock_t clock)
{
    extern uint64_t __bdk_clock_get_rate_slow(bdk_node_t node, bdk_clock_t clock) __attribute__ ((pure));
    if (clock == BDK_CLOCK_TIME)
        return BDK_GTI_RATE; /* Programed as part of setup */
    else
        return __bdk_clock_get_rate_slow(node, clock);
}

/** @} */
