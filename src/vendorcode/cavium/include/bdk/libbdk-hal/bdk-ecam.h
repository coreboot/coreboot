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
 * Interface to the ECAMs.
 *
 * @addtogroup hal
 * @{
 */

/**
 * Read from an ECAM
 *
 * @param device Device to read from
 * @param reg    Register to read
 *
 * @return Result of the read of -1 on failure
 */
extern uint32_t bdk_ecam_read32(const bdk_device_t *device, int reg);

/**
 * Write to an ECAM register
 *
 * @param device Device to write to
 * @param reg    Register to write
 * @param value  Value to write
 */
extern void bdk_ecam_write32(const bdk_device_t *device, int reg, uint32_t value);

/**
 * Build an ECAM config space request address for a device
 *
 * @param device Device being accessed
 * @param reg    Register to access
 *
 * @return 64bit IO address
 */
extern uint64_t __bdk_ecam_build_address(const bdk_device_t *device, int reg);

/**
 * Return the number of internal ECAMS on a node.
 *
 * @param node   Node to query
 *
 * @return Number of ECAMs available
 */
extern int bdk_ecam_get_num(bdk_node_t node);

/**
 * Scan all ECAMs for devices and add them to bdk-device
 *
 * @param node   Node to scan
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_ecam_scan_all(bdk_node_t node) BDK_WEAK;

/** @} */

