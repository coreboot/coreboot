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
#include <string.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-l2c.h>

BDK_REQUIRE_DEFINE(DRAM_CONFIG);

/**
 * Lookup a DRAM configuration by name and initialize DRAM using it
 *
 * @param node   Node to configure
 * @param ddr_clock_override
 *               If non zero, override the DRAM frequency specified
 *               in the config with this value
 *
 * @return Amount of DRAM in MB, or negative on failure
 */
int bdk_dram_config(int node, int ddr_clock_override)
{
    const dram_config_t *config = libdram_config_load(node);
    if (!config)
    {
        printf("N%d: No DRAM config specified, skipping DRAM init\n", node);
        return 0;
    }

    BDK_TRACE(DRAM, "N%d: Starting DRAM init (config=%p, ddr_clock_override=%d)\n", node, config, ddr_clock_override);
    int mbytes = libdram_config(node, config, ddr_clock_override);
    BDK_TRACE(DRAM, "N%d: DRAM init returned %d\n", node, mbytes);
    if (mbytes <= 0)
    {
        printf("ERROR: DDR initialization failed\n");
        return -1;
    }

    return mbytes;
}

/**
 * Do all the DRAM Margin tests 
 *
 * @param node   Node to test
 *
 * @return Success or Fail
 */
void bdk_dram_margin(int node)
{
    BDK_TRACE(DRAM, "N%d: Starting DRAM margining\n", node);
    libdram_margin(node);
    BDK_TRACE(DRAM, "N%d: Finished DRAM margining.\n", node);
    return;
}

/**
 * Return the string of the DRAM configuration info at the specified node.
 * If the node is not configured, NULL is returned.
 *
 * @param node   node to retrieve
 *
 * @return string or NULL
 */
const char* bdk_dram_get_info_string(int node)
{
    #define INFO_STRING_LEN 40
    static char info_string[INFO_STRING_LEN];
    static const char *info_ptr = info_string;

    snprintf(info_string, INFO_STRING_LEN,
             " %ld MB, %ld MT/s, %s %s",
             bdk_dram_get_size_mbytes(node),
             libdram_get_freq_from_pll(node, 0) / 1000000,
             (__bdk_dram_is_ddr4(node, 0)) ? "DDR4" : "DDR3",
             (__bdk_dram_is_rdimm(node, 0)) ? "RDIMM" : "UDIMM");

    return info_ptr;
}


/**
 * Return the highest address currently used by the BDK. This address will
 * be about 4MB above the top of the BDK to make sure small growths between the
 * call and its use don't cause corruption. Any call to memory allocation can
 * change this value.
 *
 * @return Size of the BDK in bytes
 */
uint64_t bdk_dram_get_top_of_bdk(void)
{
    /* Make sure the start address is higher that the BDK's active range.
     *
     * As sbrk() returns a node address, mask off the node portion of
     * the address to make it a physical offset. Doing this simplifies the
     * address checks and calculations which only work with physical offsets.
     */
	/* FIXME(dhendrix): we only care about node 0 */
//    uint64_t top_of_bdk = (bdk_ptr_to_phys(sbrk(0)) & bdk_build_mask(40));
    uint64_t top_of_bdk = 0;
    uint64_t l2_size = bdk_l2c_get_cache_size_bytes(bdk_numa_master());
    if (top_of_bdk <= l2_size)
    {
        /* Early BDK code takes care of the first L2 sized area of memory */
        top_of_bdk = l2_size;
    }
    else
    {
        /* Give 4MB of extra so the BDK has room to grow */
        top_of_bdk += 4 << 20;
        /* Align it on a 64KB boundary */
        top_of_bdk >>= 16;
        top_of_bdk <<= 16;
    }
    return top_of_bdk;
}
