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
#include <libbdk-hal/bdk-utils.h>


/**
 * Return the number of LMC controllers in use
 *
 * @param node   Node to probe
 *
 * @return 2 or 4 depending on the mode
 */
int __bdk_dram_get_num_lmc(bdk_node_t node)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        BDK_CSR_INIT(lmcx_dll_ctl2, node, BDK_LMCX_DLL_CTL2(2)); // sample LMC2
        return (lmcx_dll_ctl2.s.intf_en) ? 4 : 2;
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
    {
        BDK_CSR_INIT(lmcx_dll_ctl1, node, BDK_LMCX_DLL_CTL2(1)); // sample LMC1
        return (lmcx_dll_ctl1.s.intf_en) ? 2 : 1;
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
    {
        return 1;
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN93XX))
    {
        BDK_CSR_INIT(lmcx_dll_ctl1, node, BDK_LMCX_DLL_CTL2(2));
        if (lmcx_dll_ctl1.s.intf_en)
            return 3;
        lmcx_dll_ctl1.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL2(1));
        return (lmcx_dll_ctl1.s.intf_en) ? 2 : 1;
    }
    bdk_error("__bdk_dram_get_num_lmc() needs update for this chip\n");
    return 1;
}

/**
 * Return whether the node/LMC is in DRESET
 *
 * @param node   Node to probe
 * @param node   LMC to probe
 *
 * @return 1 or 0
 */
static int __bdk_dram_is_lmc_in_dreset(bdk_node_t node, int lmc)
{
    BDK_CSR_INIT(lmcx_dll_ctl2, node, BDK_LMCX_DLL_CTL2(lmc)); // can always read this
    return (lmcx_dll_ctl2.s.dreset != 0) ? 1 : 0;
}

/**
 * Return a mask of the number of row bits in use
 *
 * @param node   Node to probe
 *
 */
uint32_t __bdk_dram_get_row_mask(bdk_node_t node, int lmc)
{
    // PROTECT!!!
    if (__bdk_dram_is_lmc_in_dreset(node, lmc)) // check LMCn
        return 0;
    BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(lmc)); // sample LMCn
    int numbits = 14 + lmcx_config.s.pbank_lsb - lmcx_config.s.row_lsb - lmcx_config.s.rank_ena;
    return ((1ul << numbits) - 1);
}

/**
 * Return a mask of the number of column bits in use
 *
 * @param node   Node to probe
 *
 */
uint32_t __bdk_dram_get_col_mask(bdk_node_t node, int lmc)
{
    // PROTECT!!!
    if (__bdk_dram_is_lmc_in_dreset(node, lmc)) // check LMCn
        return 0;
    BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(lmc)); // sample LMCn
    int numbits = 11 + lmcx_config.s.row_lsb - __bdk_dram_get_num_bank_bits(node, lmc);
    return ((1ul << numbits) - 1);
}

/**
 * Return the number of bank bits in use
 *
 * @param node   Node to probe
 *
 */
// all DDR3, and DDR4 x16 today, use only 3 bank bits; DDR4 x4 and x8 always have 4 bank bits
// NOTE: this will change in the future, when DDR4 x16 devices can come with 16 banks!! FIXME!!
int __bdk_dram_get_num_bank_bits(bdk_node_t node, int lmc)
{
    // PROTECT!!!
    if (__bdk_dram_is_lmc_in_dreset(node, lmc)) // check LMCn
        return 0;
    BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(lmc)); // sample LMCn
    int bank_width = (__bdk_dram_is_ddr4(node, lmc) && (lmcx_config.s.bg2_enable)) ? 4 : 3; 
    return bank_width;
}

/**
 * Return whether the node has DDR3 or DDR4 DRAM
 *
 * @param node   Node to probe
 *
 * @return 0 (DDR3) or 1 (DDR4)
 */
int __bdk_dram_is_ddr4(bdk_node_t node, int lmc)
{
    // PROTECT!!!
    if (__bdk_dram_is_lmc_in_dreset(node, lmc)) // check LMCn
        return 0;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 1;
    BDK_CSR_INIT(lmcx_ddr_pll_ctl, node, BDK_LMCX_DDR_PLL_CTL(lmc)); // sample LMCn
    return (lmcx_ddr_pll_ctl.cn83xx.ddr4_mode != 0);
}

/**
 * Return whether the node has Registered DIMMs or Unbuffered DIMMs
 *
 * @param node   Node to probe
 *
 * @return 0 (Unbuffered) or 1 (Registered)
 */
int __bdk_dram_is_rdimm(bdk_node_t node, int lmc)
{
    // PROTECT!!!
    if (__bdk_dram_is_lmc_in_dreset(node, lmc)) // check LMCn
        return 0;
    BDK_CSR_INIT(lmcx_control, node, BDK_LMCX_CONTROL(lmc)); // sample LMCn
    return (lmcx_control.s.rdimm_ena != 0);
}

/**
 * Get the amount of DRAM configured for a node. This is read from the LMC
 * controller after DRAM is setup.
 *
 * @param node   Node to query
 *
 * @return Size in megabytes
 */
uint64_t bdk_dram_get_size_mbytes(int node)
{
    /* Return zero if dram isn't enabled */
    if (!__bdk_is_dram_enabled(node))
        return 0;

    uint64_t memsize = 0;
    const int num_dram_controllers = __bdk_dram_get_num_lmc(node);
    for (int lmc = 0; lmc < num_dram_controllers; lmc++)
    {
         // PROTECT!!!
        if (__bdk_dram_is_lmc_in_dreset(node, lmc)) // check LMCn
            return 0;
        BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(lmc));
        int num_ranks = bdk_pop(lmcx_config.s.init_status);
        uint64_t rank_size = 1ull << (28 + lmcx_config.s.pbank_lsb - lmcx_config.s.rank_ena);
        memsize += rank_size * num_ranks;
    }
    return memsize >> 20;
}

