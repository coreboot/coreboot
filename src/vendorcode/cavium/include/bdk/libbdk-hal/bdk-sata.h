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
 * This file provides a SATA driver
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @addtogroup hal
 * @{
 */

/**
 * SATA pattern generation and loopback supports a number of
 * modes. This enumeration describes the modes used by
 * bdk_sata_bist_fir().
 */
typedef enum
{
    BDK_SATA_BIST_FIS_RETIMED, /* Send FIS to tell device to enter Retimed loopback */
    BDK_SATA_BIST_FIS_ANALOG,  /* Send FIS to tell device to enter Analog loopback */
    BDK_SATA_BIST_FIS_TX_ONLY, /* Send FIS to tell device to transit only */
    BDK_SATA_BIST_SW_RETIMED,  /* No FIS, just enter local retimed loopback */
    BDK_SATA_BIST_SW_TX_ONLY_SSOP,  /* No FIS, just enter local transit only, SSOP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_HTDP,  /* No FIS, just enter local transit only, HTDP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_LTDP,  /* No FIS, just enter local transit only, LTDP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_LFSCP, /* No FIS, just enter local transit only, LFSCP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_COMP,  /* No FIS, just enter local transit only, COMP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_LBP,   /* No FIS, just enter local transit only, LBP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_MFTP,  /* No FIS, just enter local transit only, MFTP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_HFTP,  /* No FIS, just enter local transit only, HFTP pattern */
    BDK_SATA_BIST_SW_TX_ONLY_LFTP,  /* No FIS, just enter local transit only, LFTP pattern */
} bdk_sata_bist_fis_t;

/**
 * Return the number of SATA controllers on the chip
 *
 * @param node   Node to query
 *
 * @return Number of controllers, could be zero.
 */
int bdk_sata_get_controllers(bdk_node_t node);

/**
 * Initialize a SATA controller and begin device detection
 *
 * @param node       Node to initialize
 * @param controller Which controller to initialize
 *
 * @return Zero on success, negative on failure
 */
int bdk_sata_initialize(bdk_node_t node, int controller);

/**
 * Shutdown a SATA controller
 *
 * @param node       Node to access
 * @param controller Controller to shutdown
 *
 * @return Zero on success, negative on failure
 */
int bdk_sata_shutdown(bdk_node_t node, int controller);

/**
 * Return the number of SATA ports connected to this AHCI controller
 *
 * @param node       Node to query
 * @param controller SATA controller
 *
 * @return Number of ports. Zero if the controller doesn't connect to a QLM.
 */
int bdk_sata_get_ports(bdk_node_t node, int controller);

/**
 * Identify the SATA device connected to a controller
 *
 * @param node       Node to query
 * @param controller Controller to query
 * @param port       Which SATA port on the controller, zero based
 *
 * @return Size of the disk in bytes
 */
uint64_t bdk_sata_identify(bdk_node_t node, int controller, int port);

/**
 * Read data from a SATA device
 *
 * @param node       Node the controller is on
 * @param controller Which controller
 * @param port       Which port on the controller, zero based
 * @param lba        48 bit Block address to read
 * @param sectors    Number of 512 bytes sectors to read
 * @param buffer     Buffer to receive the data. Must be at least 512 * sectors in size
 *
 * @return Zero on success, negative on failure
 */
int bdk_sata_read(bdk_node_t node, int controller, int port, uint64_t lba, int sectors, void *buffer);

/**
 * Write data to a SATA device
 *
 * @param node       Node the controller is on
 * @param controller Which controller
 * @param port       Which port on the controller, zero based
 * @param lba        48 bit Block address to write
 * @param sectors    Number of 512 bytes sectors to write
 * @param buffer     Data buffer to write. Must be at least 512 * sectors in size
 *
 * @return Zero on success, negative on failure
 */
int bdk_sata_write(bdk_node_t node, int controller, int port, uint64_t lba, int sectors, const void *buffer);

/**
 * Enter one of the SATA pattern generation / loop testing modes
 *
 * @param node       Node to access
 * @param controller SATA controller to access
 * @param port       Which port on the controller
 * @param mode       Test mode to enter
 *
 * @return Zero on success, negative on failure
 */
int bdk_sata_bist_fis(bdk_node_t node, int controller, int port, bdk_sata_bist_fis_t mode);

/** @} */
