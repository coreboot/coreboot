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
 * Interface to the PKO.
 *
 * @addtogroup hal
 * @{
 */

/* Maximum number of segments which fit flat lmtstore operation.
   1) LMTST for PKO can be a maximum of 15 64bit words
   2) PKO descriptors are 2 64bit words each
   3) Every send requires PKO_SEND_HDR_S for hardware
   So 15 words / 2 = 7 possible descriptors
   7 - HDR  = 6 descriptors left for GATHER */
#define BDK_PKO_SEG_LIMIT 6

/**
 * Perform global init of PKO
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pko_global_init(bdk_node_t node);

/**
 * Configure PKO for a specific port. This is called for each
 * port on every interface that connects to PKO.
 *
 * @param handle Handle for port to config
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pko_port_init(bdk_if_handle_t handle);

/**
 * Enable PKO after all setup is complete
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pko_enable(bdk_node_t node);

/**
 * Get the current TX queue depth. Note that this operation may be slow
 * and adversly affect packet IO performance.
 *
 * @param handle Port to check
 *
 * @return Depth of the queue in packets
 */
extern int bdk_pko_get_queue_depth(bdk_if_handle_t handle);

/**
 * Set PKO shapping as a specific queue level
 *
 * @param node       Node to shape
 * @param queue      Queue to shape
 * @param level      Level in PKO
 * @param is_red     Non-zero of the rate is for the yellow/red transition. Zero for the
 *                   green/yellow transition.
 * @param is_packets Non-zero if the rate is packets/sec, otherwise rate is bits/sec
 * @param rate       Desired rate. A rate of zero disables shaping
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pko_shape(bdk_node_t node, int queue, int level, int is_red, int is_packets, uint64_t rate);

/**
 * Send a packet
 *
 * @param handle Handle of port to send on
 * @param packet Packet to send
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_pko_transmit(bdk_if_handle_t handle, const bdk_if_packet_t *packet);

/**
 * Query PKO and fill in the receive stats for the supplied
 * interface handle. The interface must use PKO for TX.
 *
 * @param handle Port handle
 *
 * @return
 */
extern void bdk_pko_fill_tx_stats(bdk_if_handle_t handle);

/** @} */
