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
#include "libbdk-arch/bdk-csrs-l2c.h"

#define EXTRACT(v, lsb, width) (((v) >> (lsb)) & ((1ull << (width)) - 1))
#define INSERT(a, v, lsb, width) a|=(((v) & ((1ull << (width)) - 1)) << (lsb))

/**
 * Given a physical DRAM address, extract information about the node, LMC, DIMM,
 * prank, lrank, bank, row, and column that was accessed.
 *
 * @param address Physical address to decode
 * @param node    Node the address was for
 * @param lmc     LMC controller the address was for
 * @param dimm    DIMM the address was for
 * @param prank   Physical RANK on the DIMM
 * @param lrank   Logical RANK on the DIMM
 * @param bank    BANK on the DIMM
 * @param row     Row on the DIMM
 * @param col     Column on the DIMM
 */
void
bdk_dram_address_extract_info(uint64_t address, int *node, int *lmc, int *dimm,
                              int *prank, int *lrank, int *bank, int *row, int *col)
{
    int bitno = CAVIUM_IS_MODEL(CAVIUM_CN83XX) ? 19 : 20;
    *node = EXTRACT(address, 40, 2); /* Address bits [41:40] */
    /* Determine the LMC controller */
    BDK_CSR_INIT(l2c_ctl, *node, BDK_L2C_CTL);
    int bank_lsb, xbits;

    /* xbits depends on number of LMCs */
    xbits = __bdk_dram_get_num_lmc(*node) >> 1; // 4->2; 2->1; 1->0
    bank_lsb = 7 + xbits;

    /* LMC number is probably aliased */
    if (l2c_ctl.s.disidxalias)
	*lmc = EXTRACT(address, 7, xbits);
    else
	*lmc = EXTRACT(address, 7, xbits) ^ EXTRACT(address, bitno, xbits) ^ EXTRACT(address, 12, xbits);

    /* Figure out the bank field width */
    BDK_CSR_INIT(lmcx_config, *node, BDK_LMCX_CONFIG(*lmc));
    int bank_width = __bdk_dram_get_num_bank_bits(*node, *lmc);

    /* Extract additional info from the LMC_CONFIG CSR */
    BDK_CSR_INIT(ext_config, *node, BDK_LMCX_EXT_CONFIG(*lmc));
    int dimm_lsb    = 28 + lmcx_config.s.pbank_lsb + xbits;
    int dimm_width  = 40 - dimm_lsb;
    int prank_lsb    = dimm_lsb - lmcx_config.s.rank_ena;
    int prank_width  = dimm_lsb - prank_lsb;
    int lrank_lsb    = prank_lsb - ext_config.s.dimm0_cid;
    int lrank_width  = prank_lsb - lrank_lsb;
    int row_lsb     = 14 + lmcx_config.s.row_lsb + xbits;
    int row_width   = lrank_lsb - row_lsb;
    int col_hi_lsb  = bank_lsb + bank_width;
    int col_hi_width= row_lsb - col_hi_lsb;

    /* Extract the parts of the address */
    *dimm =  EXTRACT(address, dimm_lsb, dimm_width);
    *prank = EXTRACT(address, prank_lsb, prank_width);
    *lrank = EXTRACT(address, lrank_lsb, lrank_width);
    *row =   EXTRACT(address, row_lsb, row_width);

    /* bank calculation may be aliased... */
    BDK_CSR_INIT(lmcx_control, *node, BDK_LMCX_CONTROL(*lmc));
    if (lmcx_control.s.xor_bank)
        *bank = EXTRACT(address, bank_lsb, bank_width) ^ EXTRACT(address, 12 + xbits, bank_width);
    else
        *bank = EXTRACT(address, bank_lsb, bank_width);

    /* LMC number already extracted */
    int col_hi = EXTRACT(address, col_hi_lsb, col_hi_width);
    *col = EXTRACT(address, 3, 4) | (col_hi << 4);
    /* Bus byte is address bits [2:0]. Unused here */
}

/**
 * Construct a physical address given the node, LMC, DIMM, prank, lrank, bank, row, and column.
 *
 * @param node    Node the address was for
 * @param lmc     LMC controller the address was for
 * @param dimm    DIMM the address was for
 * @param prank   Physical RANK on the DIMM
 * @param lrank   Logical RANK on the DIMM
 * @param bank    BANK on the DIMM
 * @param row     Row on the DIMM
 * @param col     Column on the DIMM
 */
uint64_t
bdk_dram_address_construct_info(bdk_node_t node, int lmc, int dimm,
                                int prank, int lrank, int bank, int row, int col)

{
    uint64_t address = 0;
    int bitno = CAVIUM_IS_MODEL(CAVIUM_CN83XX) ? 19 : 20;

    // insert node bits
    INSERT(address, node, 40, 2); /* Address bits [41:40] */

    /* xbits depends on number of LMCs */
    int xbits = __bdk_dram_get_num_lmc(node) >> 1; // 4->2; 2->1; 1->0
    int bank_lsb = 7 + xbits;

    /* Figure out the bank field width */
    int bank_width = __bdk_dram_get_num_bank_bits(node, lmc);

    /* Extract additional info from the LMC_CONFIG CSR */
    BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(lmc));
    BDK_CSR_INIT(ext_config, node, BDK_LMCX_EXT_CONFIG(lmc));
    int dimm_lsb     = 28 + lmcx_config.s.pbank_lsb + xbits;
    int dimm_width   = 40 - dimm_lsb;
    int prank_lsb    = dimm_lsb - lmcx_config.s.rank_ena;
    int prank_width  = dimm_lsb - prank_lsb;
    int lrank_lsb    = prank_lsb - ext_config.s.dimm0_cid;
    int lrank_width  = prank_lsb - lrank_lsb;
    int row_lsb      = 14 + lmcx_config.s.row_lsb + xbits;
    int row_width    = lrank_lsb - row_lsb;
    int col_hi_lsb   = bank_lsb + bank_width;
    int col_hi_width = row_lsb - col_hi_lsb;

    /* Insert some other parts of the address */
    INSERT(address, dimm, dimm_lsb, dimm_width);
    INSERT(address, prank, prank_lsb, prank_width);
    INSERT(address, lrank, lrank_lsb, lrank_width);
    INSERT(address, row,  row_lsb,  row_width);
    INSERT(address, col >> 4, col_hi_lsb, col_hi_width);
    INSERT(address, col, 3, 4);

    /* bank calculation may be aliased... */
    BDK_CSR_INIT(lmcx_control, node, BDK_LMCX_CONTROL(lmc));
    int new_bank = bank;
    if (lmcx_control.s.xor_bank)
        new_bank ^= EXTRACT(address, 12 + xbits, bank_width);
    INSERT(address, new_bank, bank_lsb, bank_width);
      
    /* Determine the actual C bits from the input LMC controller arg */
    /* The input LMC number was probably aliased with other fields */
    BDK_CSR_INIT(l2c_ctl, node, BDK_L2C_CTL);
    int new_lmc = lmc;
    if (!l2c_ctl.s.disidxalias)
	new_lmc ^= EXTRACT(address, bitno, xbits) ^ EXTRACT(address, 12, xbits);
    INSERT(address, new_lmc, 7, xbits);

    return address;
}
