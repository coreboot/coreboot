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
 * Interface to the MMC, eMMC, or SD
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @addtogroup hal
 * @{
 */

/**
 * Initialize a MMC for read/write
 *
 * @author creese (10/14/2013)
 * @param chip_sel Chip select to use
 *
 * @return Size of the SD card, zero on failure
 */
extern int64_t bdk_mmc_initialize(bdk_node_t node, int chip_sel);

/**
 * Read blocks from a MMC card
 *
 * @author creese (10/14/2013)
 * @param node     Node to access
 * @param chip_sel Chip select to use
 * @param address  Offset into the card in bytes. Must be a multiple of 512
 * @param buffer   Buffer to read into
 * @param length   Length to read in bytes. Must be a multiple of 512
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_mmc_read(bdk_node_t node, int chip_sel, uint64_t address, void *buffer, int length);

/**
 * Write blocks to a MMC card
 *
 * @author creese (10/14/2013)
 * @param node     Node to access
 * @param chip_sel Chip select to use
 * @param address  Offset into the card in bytes. Must be a multiple of 512
 * @param buffer   Buffer to write
 * @param length   Length to write in bytes. Must be a multiple of 512
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_mmc_write(bdk_node_t node, int chip_sel, uint64_t address, const void *buffer, int length);

/** @} */
