#ifndef __CB_BDK_BOOT_STATUS_H__
#define __CB_BDK_BOOT_STATUS_H__
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
 * Interface to report boot status
 *
 * @addtogroup boot
 * @{
 */

/**
 * Possible boot statuses that can be reported
 */
typedef enum
{
    /* Codes for boot stub */
    BDK_BOOT_STATUS_BOOT_STUB_STARTING              = 0x000,
    BDK_BOOT_STATUS_BOOT_STUB_WAITING_FOR_KEY       = 0x001,
    BDK_BOOT_STATUS_BOOT_STUB_BOOT_MENU_KEY         = 0x102, /* Signal boot complete as stopped by user */
    BDK_BOOT_STATUS_BOOT_STUB_NO_BOOT_MENU_KEY      = 0x003,
    BDK_BOOT_STATUS_BOOT_STUB_LOAD_FAILED           = 0x004,
    /* Codes for init.bin */
    BDK_BOOT_STATUS_INIT_STARTING                   = 0x005,
    BDK_BOOT_STATUS_INIT_NODE0_DRAM                 = 0x006,
    BDK_BOOT_STATUS_INIT_NODE0_DRAM_COMPLETE        = 0x007,
    BDK_BOOT_STATUS_INIT_NODE0_DRAM_FAILED          = 0x008,
    BDK_BOOT_STATUS_INIT_CCPI                       = 0x009,
    BDK_BOOT_STATUS_INIT_CCPI_COMPLETE              = 0x00a,
    BDK_BOOT_STATUS_INIT_CCPI_FAILED                = 0x00b,
    BDK_BOOT_STATUS_INIT_NODE1_DRAM                 = 0x00c,
    BDK_BOOT_STATUS_INIT_NODE1_DRAM_COMPLETE        = 0x00d,
    BDK_BOOT_STATUS_INIT_NODE1_DRAM_FAILED          = 0x00e,
    BDK_BOOT_STATUS_INIT_QLM                        = 0x00f,
    BDK_BOOT_STATUS_INIT_QLM_COMPLETE               = 0x010,
    BDK_BOOT_STATUS_INIT_QLM_FAILED                 = 0x011,
    BDK_BOOT_STATUS_INIT_LOAD_ATF                   = 0x012,
    BDK_BOOT_STATUS_INIT_LOAD_DIAGNOSTICS           = 0x013,
    BDK_BOOT_STATUS_INIT_LOAD_FAILED                = 0x014,
    /* Codes for diagnostics.bin */
    BDK_BOOT_STATUS_DIAG_STARTING                   = 0x015,
    BDK_BOOT_STATUS_DIAG_COMPLETE                   = 0x116, /* Signal boot complete */
    /* Special codes */
    BDK_BOOT_STATUS_REQUEST_POWER_CYCLE             = 0x0f2, /* Don't continue, power cycle */
} bdk_boot_status_t;

/**
 * Report boot status to the BMC or whomever might care. This function
 * will return quickly except for a status of "power cycle". In the power cycle
 * case it is assumed the board is in a bad state and should not continue until
 * a power cycle restarts us.
 *
 * @param status Status to report. Enumerated in bdk_boot_status_t
 */
extern void bdk_boot_status(bdk_boot_status_t status);

/** @} */
#endif	/* !__CB_BDK_BOOT_STATUS_H__ */
