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
#include "libbdk-arch/bdk-csrs-gti.h"
#include "libbdk-arch/bdk-csrs-ocx.h"

/**
 * Called in __bdk_init to setup the global timer
 */
void bdk_clock_setup(bdk_node_t node)
{
    const bdk_node_t local_node = bdk_numa_local();

    /* Check if the counter was already setup */
    BDK_CSR_INIT(cntcr, node, BDK_GTI_CC_CNTCR);
    if (cntcr.s.en)
        return;

    /* Configure GTI to tick at BDK_GTI_RATE */
    uint64_t sclk = bdk_clock_get_rate(node, BDK_CLOCK_SCLK);
    uint64_t inc = (BDK_GTI_RATE << 32) / sclk;
    BDK_CSR_WRITE(node, BDK_GTI_CC_CNTRATE, inc);
    BDK_CSR_WRITE(node, BDK_GTI_CTL_CNTFRQ, BDK_GTI_RATE);
    cntcr.s.en = 1;
    if (node != local_node)
    {
        /* Synchronize with local node. Very simple set of counter, will be
           off a little */
        BDK_CSR_WRITE(node, BDK_GTI_CC_CNTCV, bdk_clock_get_count(BDK_CLOCK_TIME));
    }
    /* Enable the counter */
    BDK_CSR_WRITE(node, BDK_GTI_CC_CNTCR, cntcr.u);
    BDK_CSR_READ(node, BDK_GTI_CC_CNTCR);

    if (node != local_node)
    {
        if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
        {
            /* Assume the delay in each direction is the same, sync the counters */
            int64_t local1 = bdk_clock_get_count(BDK_CLOCK_TIME);
            int64_t remote = BDK_CSR_READ(node, BDK_GTI_CC_CNTCV);
            int64_t local2 = bdk_clock_get_count(BDK_CLOCK_TIME);
            int64_t expected = (local1 + local2) / 2;
            BDK_CSR_WRITE(node, BDK_GTI_CC_CNTADD, expected - remote);
            BDK_TRACE(INIT, "N%d.GTI: Clock synchronization with master\n"
                "    expected: %ld, remote %ld\n"
                "    Counter correction: %ld\n",
                node, expected, remote, expected - remote);
        }
        else
        {
            /* Due to errata TBD, we need to use OCX_PP_CMD to write
               GTI_CC_CNTMB in order for timestamps to update. These constants
               are the addresses we need for both local and remote GTI_CC_CNTMB */
            const uint64_t LOCAL_GTI_CC_CNTMB = bdk_numa_get_address(local_node, BDK_GTI_CC_CNTMB);
            const uint64_t REMOTE_GTI_CC_CNTMB = bdk_numa_get_address(node, BDK_GTI_CC_CNTMB);
            /* Build partial OCX_PP_CMD command used for writes. Address will
               be filled later */
            BDK_CSR_DEFINE(pp_cmd, BDK_OCX_PP_CMD);
            pp_cmd.u = 0;
            pp_cmd.s.wr_mask = 0xff;

            const int NUM_AVERAGE = 16; /* Choose a power of two to avoid division */
            int64_t local_to_remote_sum = 0;
            int64_t local_to_remote_min = 1000000;
            int64_t local_to_remote_max = -1000000;
            int64_t remote_to_local_sum = 0;
            int64_t remote_to_local_min = 1000000;
            int64_t remote_to_local_max = -1000000;
            for (int loop = 0; loop < NUM_AVERAGE; loop++)
            {
                /* Perform a write to the remote GTI_CC_CNTMB to cause timestamp
                   update. We don't care about the value actually written */
                pp_cmd.s.addr = REMOTE_GTI_CC_CNTMB;
                BDK_CSR_WRITE(local_node, BDK_OCX_PP_CMD, pp_cmd.u);
                BDK_CSR_READ(local_node, BDK_OCX_PP_CMD);

                int64_t remote = BDK_CSR_READ(node, BDK_GTI_CC_CNTMBTS);
                int64_t local = BDK_CSR_READ(local_node, BDK_GTI_CC_CNTMBTS);
                int64_t delta = remote - local;

                local_to_remote_sum += delta;
                if (delta < local_to_remote_min)
                    local_to_remote_min = delta;
                if (delta > local_to_remote_max)
                    local_to_remote_max = delta;

                /* Perform a write to the local GTI_CC_CNTMB to cause timestamp
                   update. We don't care about the value actually written */
                pp_cmd.s.addr = LOCAL_GTI_CC_CNTMB;
                BDK_CSR_WRITE(node, BDK_OCX_PP_CMD, pp_cmd.u);
                BDK_CSR_READ(node, BDK_OCX_PP_CMD);

                remote = BDK_CSR_READ(node, BDK_GTI_CC_CNTMBTS);
                local = BDK_CSR_READ(local_node, BDK_GTI_CC_CNTMBTS);
                delta = local - remote;

                remote_to_local_sum += delta;
                if (delta < remote_to_local_min)
                    remote_to_local_min = delta;
                if (delta > remote_to_local_max)
                    remote_to_local_max = delta;
            }
            /* Calculate average, rounding to nearest */
            int64_t local_to_remote = (local_to_remote_sum + NUM_AVERAGE/2) / NUM_AVERAGE;
            int64_t remote_to_local = (remote_to_local_sum + NUM_AVERAGE/2) / NUM_AVERAGE;
            /* Calculate remote node offset */
            int64_t remote_offset = (remote_to_local - local_to_remote) / 2;
            BDK_CSR_WRITE(node, BDK_GTI_CC_CNTADD, remote_offset);
            BDK_TRACE(INIT, "N%d.GTI: Clock synchronization with master\n"
                "    local -> remote: min %ld, avg %ld, max %ld\n"
                "    remote -> local: min %ld, avg %ld, max %ld\n"
                "    Counter correction: %ld\n",
                node,
                local_to_remote_min, local_to_remote, local_to_remote_max,
                remote_to_local_min, remote_to_local, remote_to_local_max,
                remote_offset);
        }
    }
}

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
    if (bdk_is_platform(BDK_PLATFORM_EMULATOR))
    {
        /* Force RCLK and SCLK to be 1GHz on emulator */
        rst_boot.s.c_mul = 20;
        rst_boot.s.pnr_mul = 20;
    }
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
    if (bdk_is_platform(BDK_PLATFORM_EMULATOR))
    {
        /* Force RCLK and SCLK to be 1GHz on emulator */
        mio_rst_boot.s.c_mul = 20;
        mio_rst_boot.s.pnr_mul = 20;
    }
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

