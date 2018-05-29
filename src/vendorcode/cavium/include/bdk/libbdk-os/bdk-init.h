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
 * Core initialization functions
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @defgroup thread Threading library
 * @{
 */

/**
 * Call this function to take secondary cores out of reset and have
 * them start running threads
 *
 * @param node     Node to use in a Numa setup. Can be an exact ID or a special
 *                 value.
 * @param coremask Cores to start. Zero is a shortcut for all.
 *
 * @return Zero on success, negative on failure.
 */
extern int bdk_init_cores(bdk_node_t node, uint64_t coremask);

/**
 * Put cores back in reset and power them down
 *
 * @param node     Node to update
 * @param coremask Each bit will be a core put in reset. Cores already in reset are unaffected
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_reset_cores(bdk_node_t node, uint64_t coremask);

/**
 * Call this function to take secondary nodes and cores out of
 * reset and have them start running threads
 *
 * @param skip_cores If non-zero, cores are not started. Only the nodes are setup
 * @param ccpi_sw_gbaud
 *                   If CCPI is in software mode, this is the speed the CCPI QLMs will be configured
 *                   for
 *
 * @return Zero on success, negative on failure.
 */
extern int bdk_init_nodes(int skip_cores, int ccpi_sw_gbaud);

/**
 * Called very early in during init of both the master and slave. It performs one
 * time init of CCPI QLM and link parameters. It must only be called once per
 * boot.
 *
 * @param is_master Non-zero if the caller is the master node
 */
extern void __bdk_init_ccpi_early(int is_master);

/**
 * Brings the CCPI lanes and links into an operational state without perofrming
 * node discovery and enumeration. After this function succeeds, CCPI lanes and
 * links are ready for traffic, but node routing has not been setup.
 *
 * Note this function runs on the slave node with the BDK code not at its link
 * address. Many normal BDK functions do not work properly. Be careful.
 *
 * @param is_master  Non-zero when run on the master node. Zero when run on the slave
 * @param gbaud      Baud rate to run links at. This is only used if the QLMs are in software init
 *                   mode. If they are strapped for hardware init, the strapping speed is used.
 * @param ccpi_trace Non-zero to enable CCPI tracing. Note that tracing doesn't use the standard
 *                   bdk-trace functions. This code runs on the secondary node before we are
 *                   multi-node, and the C library doesn't work right.
 *
 * @return Zero on success, negative on failure. Zero means CCPI lanes and links are
 *         functional.
 */
extern int __bdk_init_ccpi_connection(int is_master, uint64_t gbaud, int ccpi_trace);

/**
 * Brings the CCPI lanes and links into an operational state without enabling
 * multi-node operation. Calling this function when the CCPI links are already
 * up does nothing. This function must return zero before you can go multi-node
 * by calling bdk_init_ccpi_multinode().
 *
 * @param gbaud  Baud rate to run links at. This is only used if the QLMs are in software init
 *               mode. If they are strapped for hardware init, the strapping speed is used.
 *
 * @return Zero on success, negative on failure. Zero means all CCPI links are functional.
 */
extern int __bdk_init_ccpi_links(uint64_t gbaud) BDK_WEAK;

/**
 * Once CCPI links are operational, this function transitions the system to a
 * multi-node setup. Note that this function only performs the low level CCPI
 * details, not BDK software setup on the other nodes. Call bdk_init_nodes()
 * for high level access to multi-node.
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_init_ccpi_multinode(void) BDK_WEAK;

/**
 * This function is the first function run on all cores once the
 * threading system takes over.
 *
 * @param arg
 * @param arg1
 */
extern void __bdk_init_main(int arg, void *arg1);

/**
 * Perform one time initialization for a node. Called for each
 * node from the master node.
 */
extern void __bdk_init_node(bdk_node_t node);

/**
 * Set the baud rate on a UART
 *
 * @param node     Node to use in a Numa setup. Can be an exact ID or a special
 *                 value.
 * @param uart     uart to set
 * @param baudrate Baud rate (9600, 19200, 115200, etc)
 * @param use_flow_control
 *                 Non zero if hardware flow control should be enabled
 */
extern void bdk_set_baudrate(bdk_node_t node, int uart, int baudrate, int use_flow_control);

/**
 * Get the coremask of the cores actively running the BDK. Doesn't count cores
 * that aren't booted.
 *
 * @param node   Node to coremask the count for
 *
 * @return 64bit bitmask
 */
extern uint64_t bdk_get_running_coremask(bdk_node_t node);

/**
 * Return the number of cores actively running in the BDK for the given node
 *
 * @param node   Node to get the core count for
 *
 * @return Number of cores running. Doesn't count cores that aren't booted
 */
extern int bdk_get_num_running_cores(bdk_node_t node);

#ifndef BDK_SHOW_BOOT_BANNERS
#define BDK_SHOW_BOOT_BANNERS 1
#endif

#define BDK_UART_BAUDRATE 115200
//#define BDK_UART_BAUDRATE 921600

extern uint64_t __bdk_init_reg_x0; /* The contents of X0 when this image started */
extern uint64_t __bdk_init_reg_x1; /* The contents of X1 when this image started */

/** @}  */
