#ifndef __CB_BDK_WARN_H__
#define __CB_BDK_WARN_H__

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

#include <console/console.h>

/**
 * @file
 *
 * Functions for reporting errors and warnings.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @defgroup stdio Standard IO related functions
 * @{
 */

#define bdk_warn(format, ...)	printk(BIOS_WARNING, format, ##__VA_ARGS__)
#define bdk_error(format, ...)	printk(BIOS_ERR, format, ##__VA_ARGS__)
#define bdk_fatal(format, ...)					\
	do {							\
		printk(BIOS_CRIT, format, ##__VA_ARGS__);	\
		while (1)					\
			;					\
	} while (0)

/* The following defines control detailed tracing of various parts of the
   BDK. Each one can be enabled(1) or disabled(0) independently. These
   should be disabled unless you are trying to debug something specific */

typedef enum
{
    BDK_TRACE_ENABLE_BGX,               /* BGX networking block */
    BDK_TRACE_ENABLE_DRAM,              /* DRAM initialzation */
    BDK_TRACE_ENABLE_DRAM_TEST,         /* DRAM test code */
    BDK_TRACE_ENABLE_INIT,              /* Early initialization, before main() */
    BDK_TRACE_ENABLE_ECAM,              /* ECAM initialization */
    BDK_TRACE_ENABLE_QLM,               /* QLM related debug */
    BDK_TRACE_ENABLE_EMMC,              /* eMMC related debug */
    BDK_TRACE_ENABLE_PCIE,              /* PCIe link init */
    BDK_TRACE_ENABLE_PCIE_CONFIG,       /* PCIe config space reads / writes */
    BDK_TRACE_ENABLE_SATA,              /* SATA/AHCI related debug */
    BDK_TRACE_ENABLE_CCPI,              /* Multi-node related debug */
    BDK_TRACE_ENABLE_FATFS,             /* FatFs related debug */
    BDK_TRACE_ENABLE_MPI,               /* MPI related debug */
    BDK_TRACE_ENABLE_ENV,               /* Environment variables related debug */
    BDK_TRACE_ENABLE_FPA,               /* Free Pool Allocator */
    BDK_TRACE_ENABLE_PKI,               /* Packet Input */
    BDK_TRACE_ENABLE_PKO,               /* Packet Output */
    BDK_TRACE_ENABLE_SSO,               /* SSO */
    BDK_TRACE_ENABLE_DEVICE,            /* ECAM based device framework */
    BDK_TRACE_ENABLE_DEVICE_SCAN,       /* ECAM based device scanning detail */
    BDK_TRACE_ENABLE_NIC,               /* Virtual NIC */
    BDK_TRACE_ENABLE_FDT_OS,            /* Device tree passed to OS */
    BDK_TRACE_ENABLE_USB_XHCI,          /* USB XHCI block */
    BDK_TRACE_ENABLE_PHY,               /* Ethernet Phy drivers debug */
    __BDK_TRACE_ENABLE_LAST,            /* Must always be last value */
} bdk_trace_enable_t;

/**
 * Macro for low level tracing of BDK functions. When enabled,
 * these translate to printf() calls. The "area" is a string
 * that is appended to "BDK_TRACE_ENABLE_" to figure out which
 * enable macro to use. The macro expects a ';' after it.
 */
#define BDK_TRACE(area, format, ...) do {                           \
    if ((BDK_TRACE_ENABLE_INIT == BDK_TRACE_ENABLE_##area &&        \
        IS_ENABLED(CONFIG_CAVIUM_BDK_VERBOSE_INIT)) ||              \
        (BDK_TRACE_ENABLE_DRAM == BDK_TRACE_ENABLE_##area &&        \
         IS_ENABLED(CONFIG_CAVIUM_BDK_VERBOSE_DRAM)) ||             \
        (BDK_TRACE_ENABLE_DRAM_TEST == BDK_TRACE_ENABLE_##area &&   \
         IS_ENABLED(CONFIG_CAVIUM_BDK_VERBOSE_DRAM_TEST)) ||        \
        (BDK_TRACE_ENABLE_QLM == BDK_TRACE_ENABLE_##area &&         \
         IS_ENABLED(CONFIG_CAVIUM_BDK_VERBOSE_QLM)) ||              \
        (BDK_TRACE_ENABLE_PCIE_CONFIG == BDK_TRACE_ENABLE_##area && \
         IS_ENABLED(CONFIG_CAVIUM_BDK_VERBOSE_PCIE_CONFIG)) ||      \
        (BDK_TRACE_ENABLE_PCIE == BDK_TRACE_ENABLE_##area &&        \
         IS_ENABLED(CONFIG_CAVIUM_BDK_VERBOSE_PCIE)) ||             \
        (BDK_TRACE_ENABLE_PHY == BDK_TRACE_ENABLE_##area &&         \
         IS_ENABLED(CONFIG_CAVIUM_BDK_VERBOSE_PHY)))                \
        printk(BIOS_DEBUG, #area ": " format, ##__VA_ARGS__);       \
    } while (0)

/** @} */
#endif
