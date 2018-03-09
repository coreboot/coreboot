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
#ifndef __BDK_QLM_COMMON_H__
#define __BDK_QLM_COMMON_H__

/* Common QLM code for Thunder based processors. The following chips
    use the same style SERDES, using the code defined in this header.
        CN88XX
        CN83XX
*/

/* These constants represent the possible QLM reference clock speeds in Hz */
#define REF_100MHZ 100000000
#define REF_125MHZ 125000000
#define REF_156MHZ 156250000

/**
 * Figure out which lane mode to use for a given reference clock and GBaud
 *
 * @param mode_name String name for error messages
 * @param qlm       QlM being configured
 * @param ref_clk   Reference clock in hertz
 * @param baud_mhz  Baud rate in Mhz
 *
 * @return Lane mode or -1 on failure
 */
extern int __bdk_qlm_get_lane_mode_for_speed_and_ref_clk(const char *mode_name, int qlm, int ref_clk, int baud_mhz);

/**
 * Setup the PEM to either driver or receive reset from PRST based on RC or EP
 *
 * @param node   Node to use in a Numa setup
 * @param pem    Which PEM to setuo
 * @param is_endpoint
 *               Non zero if PEM is a EP
 */
extern void __bdk_qlm_setup_pem_reset(bdk_node_t node, int pem, int is_endpoint);

/**
 * Measure the reference clock of a QLM
 *
 * @param qlm    QLM to measure
 *
 * @return Clock rate in Hz
 */
extern int __bdk_qlm_measure_refclock(bdk_node_t node, int qlm);

/**
 * Put a QLM into hardware reset
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_reset(bdk_node_t node, int qlm);

/**
 * Enable PRBS on a QLM
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 * @param prbs   PRBS mode (31, etc)
 * @param dir    Directions to enable. This is so you can enable TX and later
 *               enable RX after TX has run for a time
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_enable_prbs(bdk_node_t node, int qlm, int prbs, bdk_qlm_direction_t dir);

/**
 * Disable PRBS on a QLM
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_disable_prbs(bdk_node_t node, int qlm);

/**
 * Return the number of PRBS errors since PRBS started running
 *
 * @param node   Node to use in numa setup
 * @param qlm    QLM to use
 * @param lane   Which lane
 * @param clear  Clear counter after return the current value
 *
 * @return Number of errors
 */
extern uint64_t __bdk_qlm_get_prbs_errors(bdk_node_t node, int qlm, int lane, int clear);

/**
 * Inject an error into PRBS
 *
 * @param node   Node to use in numa setup
 * @param qlm    QLM to use
 * @param lane   Which lane
 */
extern void __bdk_qlm_inject_prbs_error(bdk_node_t node, int qlm, int lane);

/**
 * Enable shallow loopback on a QLM
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 * @param loop   Type of loopback. Not all QLMs support all modes
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_enable_loop(bdk_node_t node, int qlm, bdk_qlm_loop_t loop);

/**
 * Initialize the QLM mode table
 *
 * @param node    Node to initialize
 * @param qlm     Which QLM
 * @param ref_clk Reference clock of the QLM in Hz
 */
extern void __bdk_qlm_init_mode_table(bdk_node_t node, int qlm, int ref_clk);

/**
 * Given a valid PEM number, return its speed in Gbaud
 *
 * @param node   Node to use in numa setup
 * @param pem    PEM to get speed of
 *
 * @return Speed in Gbaud. Zero if disabled
 */
extern int __bdk_qlm_get_gbaud_mhz_pem(bdk_node_t node, int pem);

/**
 * Get the speed of a QLM using its LMODE. This can't be used on PCIe QLMs.
 *
 * @param node   Node to use in numa setup
 * @param qlm    Which QLM
 *
 * @return QLM speed on Gbaud
 */
extern int __bdk_qlm_get_gbaud_mhz_lmode(bdk_node_t node, int qlm);

/**
 * Converts a measured reference clock to a likely ideal value. Rounds
 * clock speed to the nearest REF_*Mhz define.
 *
 * @param node   Node to use in numa setup
 * @param qlm    Which QLM
 * @param measured_hz
 *               Measured value
 *
 * @return Value exactly matching a define
 */
extern int __bdk_qlm_round_refclock(bdk_node_t node, int qlm, int measured_hz);

/**
 * For Cavium EVB and EBB board, query the MCU to determine the QLM setup. Applying
 * any configuration found.
 *
 * @param node   Node to configure
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_mcu_auto_config(bdk_node_t node);

/**
 * Display the current settings of a QLM lane
 *
 * @param node     Node the QLM is on
 * @param qlm      QLM to display
 * @param qlm_lane Lane to use
 * @param show_tx  Display TX parameters
 * @param show_rx  Display RX parameters
 */
extern void bdk_qlm_display_settings(bdk_node_t node, int qlm, int qlm_lane, bool show_tx, bool show_rx);

/**
 * Perform RX equalization on a QLM
 *
 * @param node     Node the QLM is on
 * @param qlm      QLM to perform RX equalization on
 * @param qlm_lane Lane to use, or -1 for all lanes
 *
 * @return Zero on success, negative if any lane failed RX equalization
 */
extern int __bdk_qlm_rx_equalization(bdk_node_t node, int qlm, int qlm_lane);

/**
 * Configure the TX tuning parameters for a QLM lane. The tuning parameters can
 * be specified as -1 to maintain their current value
 *
 * @param node      Node to configure
 * @param qlm       QLM to configure
 * @param lane      Lane to configure
 * @param tx_swing  Transmit swing (coef 0) Range 0-31
 * @param tx_pre    Pre cursor emphasis (Coef -1). Range 0-15
 * @param tx_post   Post cursor emphasis (Coef +1). Range 0-31
 * @param tx_gain   Transmit gain. Range 0-7
 * @param tx_vboost Transmit voltage boost. Range 0-1
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_tune_lane_tx(bdk_node_t node, int qlm, int lane, int tx_swing, int tx_pre, int tx_post, int tx_gain, int tx_vboost);

/**
 * Some QLM speeds need to override the default tuning parameters
 *
 * @param node     Node to use in a Numa setup
 * @param qlm      QLM to configure
 * @param mode     Desired mode
 * @param baud_mhz Desired speed
 */
extern void __bdk_qlm_tune(bdk_node_t node, int qlm, bdk_qlm_modes_t mode, int baud_mhz);

/**
 * Capture an eye diagram for the given QLM lane. The output data is written
 * to "eye".
 *
 * @param node     Node to use in numa setup
 * @param qlm      QLM to use
 * @param qlm_lane Which lane
 * @param eye      Output eye data
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_eye_capture_cn8xxx(bdk_node_t node, int qlm, int lane, bdk_qlm_eye_t *eye_data) BDK_WEAK;

/**
 * Disables DFE for the specified QLM lane(s).
 * This function should only be called for low-loss channels.
 *
 * @param node   Node to configure
 * @param qlm    QLM to configure
 * @param lane   Lane to configure, or -1 for all lanes
 */
extern void __bdk_qlm_dfe_disable(int node, int qlm, int lane);

/**
 * Check if a specific lane is using KR training. This is used by low level GSER
 * code to remember which QLMs and lanes need to support KR training for BGX. The
 * hardware doesn't have a bit set aside to record this, so we repurpose the
 * register GSERX_SCRATCH.
 *
 * @param node   Node to check
 * @param qlm    QLM to check
 * @param lane   Lane to check
 *
 * @return True if this lane uses KR with BGX, false otherwise
 */
extern bool __bdk_qlm_is_lane_kr(bdk_node_t node, int qlm, int lane);

/**
 * Set if a specific lane is using KR training. This is used by low level GSER
 * code to remember which QLMs and lanes need to support KR training for BGX. The
 * hardware doesn't have a bit set aside to record this, so we repurpose the
 * register GSERX_SCRATCH.
 *
 * @param node   Node to set
 * @param qlm    QLM to set
 * @param lane   Lane to set
 * @param is_kr  KR (true) or XFI/XLAUI (false)
 */
extern void __bdk_qlm_set_lane_kr(bdk_node_t node, int qlm, int lane, bool is_kr);

/**
 * Initialize a DLM/QLM for use with SATA controllers
 *
 * @param node       Node to intialize
 * @param qlm        Which DLM/QLM to init
 * @param baud_mhz   QLM speed in Gbaud
 * @param sata_first First SATA controller connected to this DLM/QLM
 * @param sata_last  Last SATA controller connected to this DLM/QLM (inclusive)
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_set_sata_cn8xxx(bdk_node_t node, int qlm, int baud_mhz, int sata_first, int sata_last);

/**
 * Initialize a DLM/QLM for use with SATA controllers
 *
 * @param node       Node to intialize
 * @param qlm        Which DLM/QLM to init
 * @param baud_mhz   QLM speed in Gbaud
 * @param sata_first First SATA controller connected to this DLM/QLM
 * @param sata_last  Last SATA controller connected to this DLM/QLM (inclusive)
 *
 * @return Zero on success, negative on failure
 */
extern int __bdk_qlm_set_sata_cn9xxx(bdk_node_t node, int qlm, int baud_mhz, int sata_first, int sata_last);

#endif /* __BDK_QLM_COMMON_H__ */
