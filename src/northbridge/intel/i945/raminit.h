/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include <types.h>

#define DIMM_SOCKETS 2

#define DIMM_TCO_BASE 0x30

/* Burst length is always 8 */
#define BURSTLENGTH	 8

struct sys_info {
	u16 memory_frequency;	/* 400, 533 or 667 */
	u16 fsb_frequency;	/* 945GM: 400, 533 or 667 / 945GC: 533, 800, or 1066 */
	u32 tclk;

	u8 trp;
	u8 trcd;
	u8 tras;
	u8 trfc;
	u8 twr;

	u8 cas;			/* 3, 4 or 5 */
	u8 refresh;		/* 0 = 15.6us, 1 = 7.8us */

	bool dual_channel;
	bool interleaved;

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
	u8 rows[2 * DIMM_SOCKETS];
	u8 cols[2 * DIMM_SOCKETS];
#define SYSINFO_DIMM_X16DS		0x00
#define SYSINFO_DIMM_X8DS		0x01
#define SYSINFO_DIMM_X16SS		0x02
#define SYSINFO_DIMM_X8DDS		0x03
#define SYSINFO_DIMM_NOT_POPULATED	0x04

	u8 banks[2 * DIMM_SOCKETS];
	u8 banksize[2 * 2 * DIMM_SOCKETS];
	const u8 *spd_addresses;

} __packed;

void receive_enable_adjust(struct sys_info *sysinfo);
void sdram_initialize(int boot_path, const u8 *sdram_addresses);
int fixup_i945gm_errata(void);
#endif				/* RAMINIT_H */
