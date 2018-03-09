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
 * Functions for information about the run platform.
 *
 * <hr>$Revision: 49448 $<hr>
 * @addtogroup hal
 * @{
 */

/**
 * This typedef defines the possible platforms for the BDK. The
 * numbers represent fuse setting in Fuses[197:195].
 */
typedef enum
{
    BDK_PLATFORM_HW = 0,
    BDK_PLATFORM_EMULATOR = 1,
    BDK_PLATFORM_RTL = 2,
    BDK_PLATFORM_ASIM = 3,
} bdk_platform_t;

/**
 * Check which platform we are currently running on. This allows a BDK binary to
 * run on various platforms without a recompile.
 *
 * @param platform Platform to check for
 *
 * @return Non zero if we are on the platform
 */
static inline int bdk_is_platform(bdk_platform_t platform) __attribute__ ((pure, always_inline));
static inline int bdk_is_platform(bdk_platform_t platform)
{
    extern bdk_platform_t __bdk_platform;
    return (__bdk_platform == platform);
}

/**
 * Call to initialize the platform state
 */
extern void __bdk_platform_init();

/** @} */
