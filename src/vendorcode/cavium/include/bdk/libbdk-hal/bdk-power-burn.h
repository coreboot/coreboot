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
typedef enum
{
    BDK_POWER_BURN_NONE,        /* Disable power burn */
    BDK_POWER_BURN_FULL,        /* Continuously burn power */
    BDK_POWER_BURN_CYCLE_10MS,  /* Cycle: Burn for 10ms, idle for 10ms */
    BDK_POWER_BURN_CYCLE_1S,    /* Cycle: Burn for 1s, idle for 1s */
    BDK_POWER_BURN_CYCLE_5S,    /* Cycle: Burn for 5s, idle for 5s */
    BDK_POWER_BURN_CYCLE_1M,    /* Cycle: Burn for 1m, idle for 1m */
    BDK_POWER_BURN_CYCLE_5M,    /* Cycle: Burn for 5m, idle for 5m */
} bdk_power_burn_type_t;

/**
 * Set the current power burn mode for a node
 *
 * @param node      Node to control power burn for
 * @param burn_type Mode of power burn
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_power_burn(bdk_node_t node, bdk_power_burn_type_t burn_type);

/**
 * Set the throttle level percent for an entire chip
 *
 * @param node     Node to set
 * @param throttle Percent of Throttle level (0-100)
 */
extern void bdk_power_throttle(bdk_node_t node, int throttle_percent);

