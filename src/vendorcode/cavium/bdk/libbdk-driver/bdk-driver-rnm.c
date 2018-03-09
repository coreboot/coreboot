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
* associateint bdk_rng_init(bdk_node_t node)
* d regulations, and may be subject to export or import
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
#include "libbdk-arch/bdk-csrs-pccpf.h"
#include "libbdk-arch/bdk-csrs-rnm.h"

#include <libbdk-hal/bdk-rng.h>
#include <libbdk-hal/device/bdk-device.h>
#define RNG_DEVID ((BDK_PCC_PROD_E_GEN << 24) | BDK_PCC_VENDOR_E_CAVIUM | (BDK_PCC_DEV_IDL_E_RNM << 16))

BDK_REQUIRE_DEFINE(RNM);

/**
 * Reads 8 bits of random data from Random number generator
 *
 * @return random data
 */
uint8_t bdk_rng_get_random8(void)
{

    return bdk_read64_uint8(bdk_numa_get_address(bdk_numa_local(), BDK_RNM_RANDOM));
}

/**
 * Reads 16 bits of random data from Random number generator
 *
 * @return random data
 */
uint16_t bdk_rng_get_random16(void)
{
    return bdk_read64_uint16(bdk_numa_get_address(bdk_numa_local(), BDK_RNM_RANDOM));
}

/**
 * Reads 32 bits of random data from Random number generator
 *
 * @return random data
 */
uint32_t bdk_rng_get_random32(void)
{
    return bdk_read64_uint32(bdk_numa_get_address(bdk_numa_local(), BDK_RNM_RANDOM));
}

/**
 * Reads 64 bits of random data from Random number generator
 *
 * @return random data
 */
uint64_t bdk_rng_get_random64(void)
{
    return bdk_read64_uint64(bdk_numa_get_address(bdk_numa_local(), BDK_RNM_RANDOM));
}

/**
 * RNM init() function
 *
 * @param device RNM to initialize
 *
 * @return Zero on success, negative on failure
 */
int bdk_rng_init(bdk_node_t node)
{
    const bdk_device_t *device = bdk_device_lookup(node, RNG_DEVID, 0);
    if (!device)
    {
        bdk_error("RNM: ECAM device not found\n");
        return -1;
    }
    BDK_BAR_MODIFY(c, device, BDK_RNM_CTL_STATUS,
        c.s.ent_en = 1;
        c.s.rng_en = 1);
    /* Read back after enable so we know it is done. Needed on t88 pass 2.0 emulator and t81 real hardware !!!! */
    BDK_BAR_READ(device, BDK_RNM_CTL_STATUS);

    /* Errata (RNM-22528) First consecutive reads to RNM_RANDOM return same
       value. Before using the random entropy, read RNM_RANDOM at least once
       and discard the data */
    bdk_rng_get_random64();
    return 0;
}

