/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * sdram_mode.h: Definitions for SDRAM Mode Register and Extended Mode Register
 */

#ifndef __SDRAMMODE_H_DEFINED
#define __SDRAMMODE_H_DEFINED

// SDRAM Mode Register definitions, per JESD79D
// These are transmitted via A0-A13

// Burst length
#define SDRAM_BURST_2     (1<<0)
#define SDRAM_BURST_4     (2<<0)
#define SDRAM_BURST_8     (3<<0)

#define SDRAM_BURST_SEQUENTIAL  (0<<3)
#define SDRAM_BURST_INTERLEAVED	(1<<3)

#define SDRAM_CAS_2_0	  (2<<4)
#define SDRAM_CAS_3_0     (3<<4)	/* Optional for DDR 200-333 */
#define SDRAM_CAS_1_5     (5<<4)	/* Optional */
#define SDRAM_CAS_2_5     (6<<4)
#define SDRAM_CAS_MASK    (7<<4)

#define SDRAM_MODE_NORMAL		(0 << 7)
#define SDRAM_MODE_TEST         (1 << 7)
#define SDRAM_MODE_DLL_RESET    (2 << 7)

// Extended Mode Register

#define SDRAM_EXTMODE_DLL_ENABLE	(0 << 0)
#define SDRAM_EXTMODE_DLL_DISABLE	(1 << 0)

#define SDRAM_EXTMODE_DRIVE_NORMAL	(0 << 1)
#define SDRAM_EXTMODE_DRIVE_WEAK	(1 << 1)	/* Optional */

#endif	// __SDRAMMODE_H_DEFINED
