#ifndef __CB_BDK_DRAM_CONFIG_H__
#define __CB_BDK_DRAM_CONFIG_H__

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
 * Functions for configuring DRAM.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @defgroup dram DRAM related functions
 * @{
 */
#if 0
/* FIXME(dhendrix): this was cavium's #include */
#include "../libdram/libdram.h"
#endif
#include <libdram/libdram.h>

/**
 * Lookup a DRAM configuration by name and intialize dram with it
 *
 * @param node   Node to configure
 * @param ddr_clock_override
 *               If non zero, override the DRAM frequency specified in the config with
 *               this value
 *
 * @return Amount of DRAM in MB, or negative on failure
 */
extern int bdk_dram_config(int node, int ddr_clock_override);

/**
 * Do DRAM configuration tuning
 *
 * @param node   Node to tune
 *
 * @return Success or Fail
 */
extern int bdk_dram_tune(int node);

/**
 * Do DRAM Margin all tests
 *
 * @param node   Node to tune
 *
 * @return None
 */
extern void bdk_dram_margin(int node);

/**
 * Get the amount of DRAM configured for a node. This is read from the LMC
 * controller after DRAM is setup.
 *
 * @param node   Node to query
 *
 * @return Size in megabytes
 */
extern uint64_t bdk_dram_get_size_mbytes(int node);

/**
 * Return the string of info about the current node's DRAM configuration.
 *
 * @param node   node to retrieve
 *
 * @return String or NULL
 */
extern const char* bdk_dram_get_info_string(int node);

/**
 * Return the highest address currently used by the BDK. This address will
 * be about 4MB above the top of the BDK to make sure small growths between the
 * call and its use don't cause corruption. Any call to memory allocation can
 * change this value.
 *
 * @return Size of the BDK in bytes
 */
extern uint64_t bdk_dram_get_top_of_bdk(void);

extern int __bdk_dram_get_num_lmc(bdk_node_t node);
extern int __bdk_dram_is_ddr4(bdk_node_t node, int lmc);
extern int __bdk_dram_is_rdimm(bdk_node_t node, int lmc);
extern uint32_t __bdk_dram_get_row_mask(bdk_node_t node, int lmc);
extern uint32_t __bdk_dram_get_col_mask(bdk_node_t node, int lmc);
extern int __bdk_dram_get_num_bank_bits(bdk_node_t node, int lmc);

/** @} */

#endif	/* !__CB_BDK_DRAM_CONFIG_H__ */
