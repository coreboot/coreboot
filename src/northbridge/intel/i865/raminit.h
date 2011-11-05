/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Jon Dufresne <jon.dufresne@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

// #ifndef NORTHBRIDGE_INTEL_I865_RAMINIT_H
// #define NORTHBRIDGE_INTEL_I865_RAMINIT_H
#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 4

/* i865 Northbridge PCI devices */
#define NORTHBRIDGE			PCI_DEV(0, 0, 0)
#define NORTHBRIDGE_MMC		PCI_DEV(0, 6, 0) // Overflow device

/*-----------------------------------------------------------------------------
Macros and definitions:
-----------------------------------------------------------------------------*/
//#define VALIDATE_DIMM_COMPATIBILITY

#define delay() udelay(200)

/* should dispose of this. */
// burst length can be 4 or 8 in single-channel or 8 in dual-channel
// interleaved bursts versus sequential bursts.. no idea about the capabilities of i865, so I left it out
//#define VG86X_MODE (SDRAM_BURST_4 | SDRAM_BURST_INTERLEAVED | SDRAM_CAS_2_5)
//#define VG86X_MODE (SDRAM_BURST_4 | SDRAM_CAS_3) // SDRAM_CAS_* == tCL?
#define VG85X_MODE (SDRAM_BURST_4 | SDRAM_BURST_INTERLEAVED | SDRAM_CAS_2_5)

/* Main Memory Control */
#define DEFAULT_MCHBAR 0xfecf0000 /* ? KB */

#define MCHBAR8(x) *((volatile u8 *)(DEFAULT_MCHBAR + x))
//#define MCHBAR16(x) *((volatile u16 *)(DEFAULT_MCHBAR + x)) // unused
#define MCHBAR32(x) *((volatile u32 *)(DEFAULT_MCHBAR + x))

/* Memory mapped registers */
#define DRB 0x0  /* DRAM Row 0-7 Boundary */
#define DRA 0x10 /* DRAM Row 0-7 Attribute */
#define DRT 0x60 /* DRAM Timing */
#define DRC 0x68 /* DRAM Controller Mode */

/* Northbridge (device 0) */
#define PAM0 0x90 /* Programmable Attribute Map #0 */
#define PAM1 0x91 /* Programmable Attribute Map #1 */
#define PAM2 0x92 /* Programmable Attribute Map #2 */
#define PAM3 0x93 /* Programmable Attribute Map #3 */
#define PAM4 0x94 /* Programmable Attribute Map #4 */
#define PAM5 0x95 /* Programmable Attribute Map #5 */
#define PAM6 0x96 /* Programmable Attribute Map #6 */

/* DRC[29] - Initialization Complete (IC) */
//#define RAM_COMMAND_IC	0x1
//#define DRC_DONE		(RAM_COMMAND_IC << 29)
#define RAM_INITIALIZATION_COMPLETE (1 << 29)

/* DRC[22:21] - Number of Channels (CHAN) */
// TODO: rename to DRC_*_CHANNEL_* ?
#define RAM_SINGLE_CHANNEL		0x0
#define RAM_DUAL_CHANNEL_LINEAR	0x1
#define RAM_DUAL_CHANNEL_TILEAR	0x2

/* DRC[10:8] - Refresh Mode Select (RMS)
 * 0x1 for Refresh interval 15.6 us
 * 0x2 for Refresh interval 7.8 us
 * 0x3 for Refresh interval 64 usec
 * 0x7 for Refresh interval 64 Clocks. (Fast Refresh Mode)
 */
// TODO: rename to DRC_RMS_* ?
// #define RAM_RMS_15_6	0x1 /* usec */
// #define RAM_RMS_7_8		0x2 /* usec */
// #define RAM_RMS_64		0x3 /* usec */
// #define RAM_RMS_FAST	0x7 /* clocks */
#define RAM_COMMAND_REFRESH 0x1

/* DRC[6:4] - Mode Select (SMS) */
// TODO: mask?
// TODO: rename to DRC_SMS_* ?
#define RAM_COMMAND_POST_RESET		0x0
#define RAM_COMMAND_NOP				0x1
#define RAM_COMMAND_PRECHARGE		0x2
#define RAM_COMMAND_MRS				0x3
#define RAM_COMMAND_EMRS			0x4
#define RAM_COMMAND_CBR				0x6
#define RAM_COMMAND_NORMAL			0x7

/* Activate to Precharge delay (tRAS) */
#define DRT_TRAS_MAX_70		(1 << 10)
#define DRT_TRAS_MAX_120	(0 << 10)
#define DRT_TRAS_MIN_MASK	(5 << 7) // conflicts with DRT_TRAS_MIN_5
#define DRT_TRAS_MIN_10		(0 << 7)
#define DRT_TRAS_MIN_9		(1 << 7)
#define DRT_TRAS_MIN_8		(2 << 7)
#define DRT_TRAS_MIN_7		(3 << 7)
#define DRT_TRAS_MIN_6		(4 << 7)
#define DRT_TRAS_MIN_5		(5 << 7)

/* CAS# latency (tCL) */
#define DRT_TCL_MASK	(3 << 5)
#define DRT_TCL_2_0		(1 << 5)
#define DRT_TCL_2_5		(0 << 5)
#define DRT_TCL_3_0		(2 << 5)

/* DRAM RAS# to CAS delay (tRCD) */
#define DRT_TRCD_MASK	(3 << 2)
#define DRT_TRCD_4		(0 << 2)
#define DRT_TRCD_3		(1 << 2)
#define DRT_TRCD_2		(2 << 2)

/* DRAM RAS# precharge (tRP) */
#define DRT_TRP_MASK	3
#define DRT_TRP_4		0
#define DRT_TRP_3		1
#define DRT_TRP_2		2

// moved from i855.h
#define DRT_CAS_MASK    (3 << 5)
#define DRT_CAS_2_0     (1 << 5)
#define DRT_CAS_2_5     (0 << 5)
#define DRT_CAS_3_0		(2 << 5)

struct sys_info { // copied from i945, not yet used
	u16 memory_frequency;	/* 400, 533 or 667 */
	u16 fsb_frequency;	/* 400, 533 or 667 */

	u8 trp;			/* calculated by sdram_detect_smallest_tRP() */
	u8 trcd;		/* calculated by sdram_detect_smallest_tRCD() */
	u8 tras;		/* calculated by sdram_detect_smallest_tRAS() */
	// u8 trfc;		/* calculated by sdram_detect_smallest_tRFC() */
	// u8 twr;			/* calculated by sdram_detect_smallest_tWR() */

	u8 cas;			/* 3, 4 or 5 */
	u8 refresh;		/* 0 = 15.6us, 1 = 7.8us */

	u8 dual_channel;	/* 0 or 1 */
	u8 interleaved;

	// u8 mvco4x;		/* 0 (8x) or 1 (4x) */
	// u8 clkcfg_bit7;
	// u8 boot_path;
// #define BOOT_PATH_NORMAL	0
// #define BOOT_PATH_RESET		1
// #define BOOT_PATH_RESUME	2

	// u8 package;		/* 0 = planar, 1 = stacked */
// #define SYSINFO_PACKAGE_PLANAR		0x00
// #define SYSINFO_PACKAGE_STACKED		0x01
	u8 dimm[2 * DIMM_SOCKETS];
#define SYSINFO_DIMM_X16DS		0x00
#define SYSINFO_DIMM_X8DS		0x01
#define SYSINFO_DIMM_X16SS		0x02
#define SYSINFO_DIMM_X8DDS		0x03
#define SYSINFO_DIMM_NOT_POPULATED	0x04

	u8 banks[2 * DIMM_SOCKETS];

	u8 banksize[2 * 2 * DIMM_SOCKETS];
	const u8 *spd_addresses;

} __attribute__ ((packed));


//void sdram_initialize(void);

#endif /* NORTHBRIDGE_INTEL_I865_RAMINIT_H */
