/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <spd.h>
#include <console/console.h>
#include "delay.h"

#include "gm45.h"

void raminit_thermal(const sysinfo_t *sysinfo)
{
	const mem_clock_t freq = sysinfo->selected_timings.mem_clock;
	int x;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, x) {
		const chip_width_t width = sysinfo->dimms[x].chip_width;
		const chip_capacity_t size = sysinfo->dimms[x].chip_capacity;
		if ((freq == MEM_CLOCK_1067MT) && (width == CHIP_WIDTH_x16)) {
			MCHBAR32(CxDTPEW(x)) = 0x0d0b0403;
			MCHBAR32(CxDTPEW(x)+4) = 0x060d;
			MCHBAR32(CxDTAEW(x)) = 0x2d0b221a;
			MCHBAR32(CxDTAEW(x)+4) = 0xc779956e;
		} else
		if ((freq == MEM_CLOCK_1067MT) && (width == CHIP_WIDTH_x8)) {
			MCHBAR32(CxDTPEW(x)) = 0x06040101;
			MCHBAR32(CxDTPEW(x)+4) = 0x0506;
			if (size == CHIP_CAP_2G)
				MCHBAR32(CxDTAEW(x)) = 0xa1071416;
			else
				MCHBAR32(CxDTAEW(x)) = 0x1a071416;
			MCHBAR32(CxDTAEW(x)+4) = 0x7246643f;
		} else
		if ((freq == MEM_CLOCK_800MT) && (width == CHIP_WIDTH_x16)) {
			MCHBAR32(CxDTPEW(x)) = 0x06030100;
			MCHBAR32(CxDTPEW(x)+4) = 0x0506;
			MCHBAR32(CxDTAEW(x)) = 0x3e081714;
			MCHBAR32(CxDTAEW(x)+4) = 0xbb79a171;
		} else
		if ((freq == MEM_CLOCK_800MT) && (width == CHIP_WIDTH_x8)) {
			if (size <= CHIP_CAP_512M)
				MCHBAR32(CxDTPEW(x)) = 0x05050101;
			else
				MCHBAR32(CxDTPEW(x)) = 0x05060101;
			MCHBAR32(CxDTPEW(x)+4) = 0x0503;
			if (size == CHIP_CAP_2G) {
				MCHBAR32(CxDTAEW(x)) = 0x57051010;
				MCHBAR32(CxDTAEW(x)+4) = 0x5fd15dde;
			} else
			if (size == CHIP_CAP_1G) {
				MCHBAR32(CxDTAEW(x)) = 0x3306130e;
				MCHBAR32(CxDTAEW(x)+4) = 0x5763485d;
			} else
			if (size <= CHIP_CAP_512M) {
				MCHBAR32(CxDTAEW(x)) = 0x1e08170d;
				MCHBAR32(CxDTAEW(x)+4) = 0x502f3827;
			}
		} else
		if ((freq == MEM_CLOCK_667MT) && (width == CHIP_WIDTH_x16)) {
			MCHBAR32(CxDTPEW(x)) = 0x02000000;
			MCHBAR32(CxDTPEW(x)+4) = 0x0402;
			MCHBAR32(CxDTAEW(x)) = 0x46061111;
			MCHBAR32(CxDTAEW(x)+4) = 0xb579a772;
		} else
		if ((freq == MEM_CLOCK_667MT) && (width == CHIP_WIDTH_x8)) {
			MCHBAR32(CxDTPEW(x)) = 0x04070101;
			MCHBAR32(CxDTPEW(x)+4) = 0x0501;
			if (size == CHIP_CAP_2G) {
				MCHBAR32(CxDTAEW(x)) = 0x32040e0d;
				MCHBAR32(CxDTAEW(x)+4) = 0x55ff59ff;
			} else
			if (size == CHIP_CAP_1G) {
				MCHBAR32(CxDTAEW(x)) = 0x3f05120a;
				MCHBAR32(CxDTAEW(x)+4) = 0x49713a6c;
			} else
			if (size <= CHIP_CAP_512M) {
				MCHBAR32(CxDTAEW(x)) = 0x20081808;
				MCHBAR32(CxDTAEW(x)+4) = 0x3f23221b;
			}
		}

		/* also L-Shaped */
		if (sysinfo->selected_timings.channel_mode ==
				CHANNEL_MODE_DUAL_INTERLEAVED) {
			if (freq == MEM_CLOCK_1067MT) {
				MCHBAR32(CxGTEW(x)) = 0xc8f81717;
			} else
			if (freq == MEM_CLOCK_800MT) {
				MCHBAR32(CxGTEW(x)) = 0x96ba1717;
			} else
			if (freq == MEM_CLOCK_667MT) {
				MCHBAR32(CxGTEW(x)) = 0x7d9b1717;
			}
		} else {
			if (freq == MEM_CLOCK_1067MT) {
				MCHBAR32(CxGTEW(x)) = 0x53661717;
			} else
			if (freq == MEM_CLOCK_800MT) {
				MCHBAR32(CxGTEW(x)) = 0x886e1717;
			} else
			if (freq == MEM_CLOCK_667MT) {
				MCHBAR32(CxGTEW(x)) = 0x38621717;
			}
		}
	}

	// always?
	MCHBAR32(CxDTC(0)) = 0x00004020;
	MCHBAR32(CxDTC(1)) = 0x00004020;
	MCHBAR32(CxGTC(0)) = 0x00304848;
	MCHBAR32(CxGTC(1)) = 0x00304848;

	/* enable thermal sensors */
	u32 tmp;
	tmp = MCHBAR32(0x1290) & 0xfff8;
	MCHBAR32(0x1290) = tmp | 0xa4810007;
	tmp = MCHBAR32(0x1390) & 0xfff8;
	MCHBAR32(0x1390) = tmp | 0xa4810007;
	tmp = MCHBAR32(0x12b4) & 0xfff8;
	MCHBAR32(0x12b4) = tmp | 0xa2810007;
	tmp = MCHBAR32(0x13b4) & 0xfff8;
	MCHBAR32(0x13b4) = tmp | 0xa2810007;
	MCHBAR8(0x1070) = 1;
	MCHBAR8(0x1080) = 6;
	if (sysinfo->gfx_type == GMCH_PM45) {
		MCHBAR16(0x1001) = 0;
		MCHBAR8(0x1007) = 0;
		MCHBAR32(0x1010) = 0;
		MCHBAR32(0x1014) = 0;
		MCHBAR8(0x101c) = 0x98;
		MCHBAR16(0x1041) = 0x9200;
		MCHBAR8(0x1047) = 0;
		MCHBAR32(0x1050) = 0x2309;
		MCHBAR32(0x1054) = 0;
		MCHBAR8(0x105c) = 0x98;
	} else {
		MCHBAR16(0x1001) = 0x9200;
		MCHBAR8(0x1007) = 0;
		MCHBAR32(0x1010) = 0x2309;
		MCHBAR32(0x1014) = 0;
		MCHBAR8(0x101c) = 0x98;
		MCHBAR16(0x1041) = 0;
		MCHBAR8(0x1047) = 0;
		MCHBAR32(0x1050) = 0;
		MCHBAR32(0x1054) = 0;
		MCHBAR8(0x105c) = 0x98;
	}

	MCHBAR32(0x1010) |= 1 << 31;
	MCHBAR32(0x1050) |= 1 << 31;
	MCHBAR32(CxGTC(0)) |= 1 << 31;
	MCHBAR32(CxGTC(1)) |= 1 << 31;

	if (sysinfo->gs45_low_power_mode) {
		MCHBAR32(0x11b0) = 0xa000083a;
	} else if (sysinfo->gfx_type == GMCH_GM49) {
		MCHBAR32(0x11b0) = 0x2000383a;
		MCHBAR16(0x1190) &= ~(1 << 15);
	} else if ((sysinfo->gfx_type != GMCH_PM45) &&
			(sysinfo->gfx_type != GMCH_UNKNOWN)) {
		MCHBAR32(0x11b0) = 0xa000383a;
	}

	switch (sysinfo->selected_timings.fsb_clock) {
		case FSB_CLOCK_667MHz:
			MCHBAR32(0x11d0) = 0x0fd88000;
			break;
		case FSB_CLOCK_800MHz:
			MCHBAR32(0x11d0) = 0x1303c000;
			break;
		case FSB_CLOCK_1067MHz:
			MCHBAR32(0x11d0) = 0x194a0000;
			break;
	}
	tmp = MCHBAR32(0x11d4) & ~0x1f;
	MCHBAR32(0x11d4) = tmp | 4;
}

