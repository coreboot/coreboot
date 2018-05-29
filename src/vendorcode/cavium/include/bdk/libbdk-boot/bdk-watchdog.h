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
 * Utility functions for controlling the watchdog during boot
 *
 * @defgroup watchdog Watchdog related functions
 * @{
 */

/**
 * Setup the watchdog to expire in timeout_ms milliseconds. When the watchdog
 * expires, the chip three things happen:
 * 1) Expire 1: interrupt that is ignored by the BDK
 * 2) Expire 2: DEL3T interrupt, which is disabled and ignored
 * 3) Expire 3: Soft reset of the chip
 *
 * Since we want a soft reset, we actually program the watchdog to expire at
 * the timeout / 3.
 *
 * @param timeout_ms Timeout in milliseconds. If this is zero, the timeout is taken from the
 *                   global configuration option BDK_BRD_CFG_WATCHDOG_TIMEOUT
 */
extern void bdk_watchdog_set(unsigned int timeout_ms);

/**
 * Signal the watchdog that we are still running
 */
extern void bdk_watchdog_poke(void);

/**
 * Disable the hardware watchdog
 */
extern void bdk_watchdog_disable(void);

/**
 * Return true if the watchdog is configured and running
 *
 * @return Non-zero if watchdog is running
 */
extern int bdk_watchdog_is_running(void);

/** @} */
