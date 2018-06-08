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
 * Interface to programming the PCIe SPI flash used for config overrides
 *
 * @addtogroup hal
 * @{
 */

#define BDK_PCIE_FLASH_PREAMBLE 0x9da1
#define BDK_PCIE_FLASH_END      0x6a5d
#define BDK_PCIE_FLASH_MAX_OFFSET 256

/**
 * Determine if access to the PCIe SPI flash is available
 *
 * @param node      Numa node request is for
 * @param pcie_port PCIe port to access
 *
 * @return One if available, zero if not
 */
extern int bdk_pcie_flash_is_available(bdk_node_t node, int pcie_port);

/**
 * Read the specified offset in the PCIe SPI flash and returns its
 * value. In the case the EEPROM isn't there or can't be read -1
 * is returned.
 *
 * @param node      Numa node request is for
 * @param pcie_port PCIe port to access
 * @param offset    Offset in bytes, Must be a multiple of 8
 *
 * @return Value read or -1 if the read failed
 */
extern uint64_t bdk_pcie_flash_read(bdk_node_t node, int pcie_port, int offset);

/**
 * Write a value to the PCIe SPI flash. The value should be of the
 * format bdk_pemx_spi_data_t.
 *
 * @param node      Numa node request is for
 * @param pcie_port PCIe port to access
 * @param offset    Offset to write. Must be a multiple of 8 bytes.
 * @param value     Value to write
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pcie_flash_write(bdk_node_t node, int pcie_port, int offset, uint64_t value);

/**
 * Erase the PCIe SPI Flash
 *
 * @param node      Numa node request is for
 * @param pcie_port PCIe port to access
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pcie_flash_erase(bdk_node_t node, int pcie_port);

/**
 * Dump the PCIe SPI Flash
 *
 * @param node      Numa node request is for
 * @param pcie_port PCIe port to access
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pcie_flash_dump(bdk_node_t node, int pcie_port);

/** @} */
