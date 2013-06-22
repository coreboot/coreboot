/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 2

#define DIMM_TCO_BASE 0x30

/* Burst length is always 8 */
#define BURSTLENGTH	 8

struct sys_info {
	u16 memory_frequency;	/* 400, 533 or 667 */
	u16 fsb_frequency;	/* 400, 533 or 667 */

	u8 trp;			/* calculated by sdram_detect_smallest_tRP() */
	u8 trcd;		/* calculated by sdram_detect_smallest_tRCD() */
	u8 tras;		/* calculated by sdram_detect_smallest_tRAS() */
	u8 trfc;		/* calculated by sdram_detect_smallest_tRFC() */
	u8 twr;			/* calculated by sdram_detect_smallest_tWR() */

	u8 cas;			/* 3, 4 or 5 */
	u8 refresh;		/* 0 = 15.6us, 1 = 7.8us */

	u8 dual_channel;	/* 0 or 1 */
	u8 interleaved;

	u8 mvco4x;		/* 0 (8x) or 1 (4x) */
	u8 clkcfg_bit7;
	u8 boot_path;
#define BOOT_PATH_NORMAL	0
#define BOOT_PATH_RESET		1
#define BOOT_PATH_RESUME	2

	u8 package;		/* 0 = planar, 1 = stacked */
#define SYSINFO_PACKAGE_PLANAR		0x00
#define SYSINFO_PACKAGE_STACKED		0x01
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

void receive_enable_adjust(struct sys_info *sysinfo);
void sdram_initialize(int boot_path, const u8 *sdram_addresses);
int fixup_i945_errata(void);
void udelay(u32 us);

#if CONFIG_DEBUG_RAM_SETUP
void sdram_dump_mchbar_registers(void);
#endif
#endif				/* RAMINIT_H */
