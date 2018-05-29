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
 * Defiens and functions for performing LMT operations, such as
 * LMTST and LMTCANCEL
 *
 * @defgroup lmt Local Memory Transaction (LMT) support
 * @{
 */
#include "libbdk-arch/bdk-csrs-lmt.h"

/**
 * Address of the LMT store area in physical memory
 */
#define __BDK_LMTLINE_CN83XX 0x87F100000000ull

/**
 * Flush the LMTLINE area of all previous writes and clear the valid flag
 */
static inline void bdk_lmt_cancel(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        *(volatile uint64_t *)(__BDK_LMTLINE_CN83XX | (1 << 10)) = 0;
    else
        BDK_CSR_WRITE(bdk_numa_local(), BDK_LMT_LF_LMTCANCEL, 0);
}

/**
 * Return a volatile pointer to the LMTLINE area in 64bit words. Good programming
 * practice would to always store sequencially, incrementing the pointer for each
 * word written.
 *
 * @return Voltaile uint64_t pointer to LMTLINE
 */
static inline volatile uint64_t *bdk_lmt_store_ptr(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return (volatile uint64_t *)__BDK_LMTLINE_CN83XX;
    else
        return (volatile uint64_t *)BDK_LMT_LF_LMTLINEX(0);
}

/**
 * Send the data stored to LMTLINE to an IO block. This call may
 * fail if the hardware has invalidated the LMTLINE area. If it
 * fails, you must issue all LMT stores again and redo this
 * call. Note the return status of this function is backwards
 * to most BDK functions. It matches the LMTST hardware result.
 *
 * @param io_address 48 bit IO address where the LMTLINE data will be sent
 *
 * @return Zero on failure, non-zero on success
 */
static inline int bdk_lmt_submit(uint64_t io_address)
{
    int64_t result = 0;
    asm volatile ("LDEOR xzr,%x[rf],[%[rs]]" : [rf] "=r"(result): [rs] "r"(io_address));
    return bdk_le64_to_cpu(result);
}

/** @} */
