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
#include <bdk.h>
#include <libbdk-arch/bdk-csrs-gti.h>
#include <libbdk-arch/bdk-csrs-ocx.h>
#include <libbdk-hal/bdk-clock.h>
#include <libbdk-arch/bdk-csrs-rst.h>

/**
 * Get cycle count based on the clock type.
 *
 * @param clock - Enumeration of the clock type.
 * @return      - Get the number of cycles executed so far.
 */
uint64_t __bdk_clock_get_count_slow(bdk_clock_t clock)
{
    bdk_node_t node = bdk_numa_local();
    BDK_CSR_INIT(rst_boot, node, BDK_RST_BOOT);
    uint64_t ref_cntr = BDK_CSR_READ(node, BDK_RST_REF_CNTR);
    switch(clock)
    {
        case BDK_CLOCK_TIME:
            return 0; /* Handled in fast path */
        case BDK_CLOCK_MAIN_REF:
            return ref_cntr;
        case BDK_CLOCK_RCLK:
            return ref_cntr * rst_boot.s.c_mul;
        case BDK_CLOCK_SCLK:
            return ref_cntr * rst_boot.s.pnr_mul;
    }
    return 0;
}

/**
 * Get clock rate based on the clock type.
 *
 * @param node    Node to use in a Numa setup. Can be an exact ID or a special value.
 * @param clock - Enumeration of the clock type.
 * @return      - return the clock rate.
 */
uint64_t __bdk_clock_get_rate_slow(bdk_node_t node, bdk_clock_t clock)
{
    /* This is currently defined to be 50Mhz */
    const uint64_t REF_CLOCK = 50000000;

    BDK_CSR_INIT(mio_rst_boot, node, BDK_RST_BOOT);
    switch (clock)
    {
        case BDK_CLOCK_TIME:
            return BDK_GTI_RATE; /* Programed as part of setup */
        case BDK_CLOCK_MAIN_REF:
            return REF_CLOCK;
        case BDK_CLOCK_RCLK:
            return REF_CLOCK * mio_rst_boot.s.c_mul;
        case BDK_CLOCK_SCLK:
            return REF_CLOCK * mio_rst_boot.s.pnr_mul;
    }
    return 0;
}

