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
#include <console/console.h>
#include <cpu/x86/msr.h>

#include "gm45.h"

static int sku_freq_index(const gmch_gfx_t sku, const int low_power_mode)
{
	if (low_power_mode)
		return 1;
	switch (sku) {
	case GMCH_GM45:
	case GMCH_GE45:
	case GMCH_GS45:
		return 0;
	case GMCH_GM47:
		return 2;
	case GMCH_GM49:
		return 3;
	default:
		return 0;
	}
}
static void init_freq_scaling(const gmch_gfx_t sku, const int low_power_mode)
{
	int i;

	MCHBAR32(0x11cc) = (MCHBAR32(0x11cc) & ~(0x1f)) | 0x17;
	switch (sku) {
	case GMCH_GM45:
	case GMCH_GE45:
	case GMCH_GS45:
	case GMCH_GM47:
	case GMCH_GM49:
		break;
	default:
		/* No more to be done for the others. */
		return;
	}

	static const u32 voltage_mask =
		(0x1f << 24) | (0x1f << 16) | (0x1f << 8) | 0x1f;
	MCHBAR32(0x1120) = (MCHBAR32(0x1120) & ~voltage_mask) | 0x10111213;
	MCHBAR32(0x1124) = (MCHBAR32(0x1124) & ~voltage_mask) | 0x14151617;
	MCHBAR32(0x1128) = (MCHBAR32(0x1128) & ~voltage_mask) | 0x18191a1b;
	MCHBAR32(0x112c) = (MCHBAR32(0x112c) & ~voltage_mask) | 0x1c1d1e1f;
	MCHBAR32(0x1130) = (MCHBAR32(0x1130) & ~voltage_mask) | 0x00010203;
	MCHBAR32(0x1134) = (MCHBAR32(0x1134) & ~voltage_mask) | 0x04050607;
	MCHBAR32(0x1138) = (MCHBAR32(0x1138) & ~voltage_mask) | 0x08090a0b;
	MCHBAR32(0x113c) = (MCHBAR32(0x113c) & ~voltage_mask) | 0x0c0d0e0f;

	/* Program frequencies. */
	static const u32 frequencies_from_sku_vco[][4][8] = {
	/* GM45/GE45/GS45_perf */ {
	    /* VCO 2666 */ { 0xcd, 0xbc, 0x9b, 0x8a, 0x79, 0x78, 0x67, 0x56 },
	    /* VCO 3200 */ { 0xcd, 0xac, 0x9b, 0x8a, 0x89, 0x78, 0x67, 0x56 },
	    /* VCO 4000 */ { 0xac, 0x9b, 0x9a, 0x89, 0x89, 0x68, 0x56, 0x45 },
	    /* VCO 5333 */ { 0xab, 0x9a, 0x79, 0x68, 0x57, 0x56, 0x45, 0x34 },
	},
	/* GS45_low_power */ {
	    /* VCO 2666 */ { 0xcd, 0x8a },
	    /* VCO 3200 */ { 0xcd, 0x89 },
	    /* VCO 4000 */ { 0xac, 0x89 },
	    /* VCO 5333 */ { 0xab, 0x68 },
	},
	/* GM47 */ {
	    /* VCO 2666 */ { 0xcd, 0xcd, 0xbc, 0x9b, 0x79, 0x78, 0x67, 0x56 },
	    /* VCO 3200 */ { 0xde, 0xcd, 0xac, 0x9b, 0x89, 0x78, 0x67, 0x56 },
	    /* VCO 4000 */ { 0xcd, 0xac, 0x9b, 0x9a, 0x89, 0x68, 0x56, 0x45 },
	    /* VCO 5333 */ { 0xac, 0xab, 0x9a, 0x79, 0x68, 0x56, 0x45, 0x34 },
	},
	/* GM49 */ {
	    /* VCO 2666 */ { },
	    /* VCO 3200 */ { 0xef, 0xde, 0xcd, 0xac, 0x89, 0x78, 0x67, 0x56 },
	    /* VCO 4000 */ { 0xef, 0xde, 0xac, 0x9b, 0x89, 0x68, 0x56, 0x45 },
	    /* VCO 5333 */ { 0xce, 0xbd, 0xab, 0x9a, 0x68, 0x57, 0x45, 0x34 },
	}};
	const int sku_index = sku_freq_index(sku, low_power_mode);
	const int vco_index = raminit_read_vco_index();
	const int reg_limit = low_power_mode ? 1 : 4;
	if (sku == GMCH_GM49)
		MCHBAR8(0x1110+3) = 0x1b;
	else
		MCHBAR8(0x1110+3) = 0x17;
	MCHBAR8(0x1110+1) = 0x17;
	if (!low_power_mode) {
		MCHBAR8(0x1114+3) = 0x17;
		MCHBAR8(0x1114+1) = 0x17;
		MCHBAR8(0x1118+3) = 0x17;
		MCHBAR8(0x1118+1) = 0x17;
		MCHBAR8(0x111c+3) = 0x17;
		MCHBAR8(0x111c+1) = 0x17;
	}
	for (i = 0; i < reg_limit; ++i) {
		const int mchbar = 0x1110 + (i * 4);
		MCHBAR8(mchbar + 2) = frequencies_from_sku_vco
					[sku_index][vco_index][i * 2 + 0];
		MCHBAR8(mchbar + 0) = frequencies_from_sku_vco
					[sku_index][vco_index][i * 2 + 1];
	}

	if (low_power_mode) {
		MCHBAR16(0x1190) =
			(MCHBAR16(0x1190) & ~((7 << 8) | (7 << 4) | 7)) |
			(1 << 8) | (1 << 4) | 1;
	} else {
		MCHBAR16(0x1190) =
			(MCHBAR16(0x1190) & ~((7 << 8) | (7 << 4))) | 7;
		if (sku == GMCH_GS45) /* performance mode */
			MCHBAR32(0x0ffc) &= ~(1 << 31);
	}

	MCHBAR16(0x0fc0) |= (1 << 11);
	MCHBAR16(0x11b8) = 0x333c;
	MCHBAR16(0x11c0 + 2) = 0x0303;
	MCHBAR32(0x11c4) = 0x0a030a03;
	MCHBAR16(0x1100) = (MCHBAR16(0x1100) & ~(0x1f << 8)) | (3 << 8);
	MCHBAR16(0x11b8 + 2) = 0x4000;
}

void init_pm(const sysinfo_t *const sysinfo)
{
	const stepping_t stepping = sysinfo->stepping;
	const fsb_clock_t fsb = sysinfo->selected_timings.fsb_clock;
	const mem_clock_t memclk = sysinfo->selected_timings.mem_clock;

	MCHBAR16(0xc14) = 0;
	MCHBAR16(0xc20) = 0;
	MCHBAR32(0xfc0) = 0x001f00fd;
	MCHBAR32(0xfc0) |= 3 << 25;
	MCHBAR32(0xfc0) |= 1 << 11;
	MCHBAR8(0xfb0) = 3;
	MCHBAR8(0xf10) |= 1 << 1;
	if (fsb == FSB_CLOCK_667MHz) {
		MCHBAR16(0xc3a) = 0xea6;
		MCHBAR8(0xc16) = (MCHBAR8(0xc16) & 0x80) | 0x0e;
	} else if (fsb == FSB_CLOCK_800MHz) {
		MCHBAR16(0xc3a) = 0x1194;
		MCHBAR8(0xc16) = (MCHBAR8(0xc16) & 0x80) | 0x10;
	} else if (fsb == FSB_CLOCK_1067MHz) {
		MCHBAR16(0xc3a) = 0x1777;
		MCHBAR8(0xc16) = (MCHBAR8(0xc16) & 0x80) | 0x15;
	}
	MCHBAR8(0xfb8) = 3;
	if (fsb == FSB_CLOCK_667MHz)
		MCHBAR16(0xc38) = 0x0ea6;
	else if (fsb == FSB_CLOCK_800MHz)
		MCHBAR16(0xc38) = 0x1194;
	else if (fsb == FSB_CLOCK_1067MHz)
		MCHBAR16(0xc38) = 0x1777;
	MCHBAR8(0xf10) |= 1 << 5;
	MCHBAR16(0xc16) |= 3 << 12;
	MCHBAR32(0xf60) = 0x01030419;
	if (fsb == FSB_CLOCK_667MHz) {
		MCHBAR32(0xf00) = 0x00000600;
		MCHBAR32(0xf04) = 0x00001d80;
	} else if (fsb == FSB_CLOCK_800MHz) {
		MCHBAR32(0xf00) = 0x00000700;
		MCHBAR32(0xf04) = 0x00002380;
	} else if (fsb == FSB_CLOCK_1067MHz) {
		MCHBAR32(0xf00) = 0x00000900;
		MCHBAR32(0xf04) = 0x00002e80;
	}
	MCHBAR16(0xf08) = 0x730f;
	if (fsb == FSB_CLOCK_667MHz)
		MCHBAR16(0xf0c) = 0x0b96;
	else if (fsb == FSB_CLOCK_800MHz)
		MCHBAR16(0xf0c) = 0x0c99;
	else if (fsb == FSB_CLOCK_1067MHz)
		MCHBAR16(0xf0c) = 0x10a4;
	MCHBAR32(0xf80) |= 1 << 31;

	MCHBAR32(0x40) = (MCHBAR32(0x40) & ~(0x3f << 24)) |
		(sysinfo->cores == 4) ? (1 << 24) : 0;

	MCHBAR32(0x40) &= ~(1 << 19);
	MCHBAR32(0x40) |= 1 << 13;
	MCHBAR32(0x40) |= 1 << 21;
	MCHBAR32(0x40) |= 1 << 9;
	if (stepping > STEPPING_B1) {
		if (fsb != FSB_CLOCK_1067MHz) {
			MCHBAR32(0x70) |= 1 << 30;
		} else {
			MCHBAR32(0x70) &= ~(1 << 30);
		}
	}
	if (stepping < STEPPING_B1)
		MCHBAR32(0x70) |= 1 << 29;
	else
		MCHBAR32(0x70) &= ~(1 << 29);
	if (stepping > STEPPING_B1) {
		MCHBAR32(0x70) |= 1 << 28;
		MCHBAR32(0x70) |= 1 << 25;
	}
	if (stepping > STEPPING_B0) {
		if (fsb != FSB_CLOCK_667MHz)
			MCHBAR32(0x70) = (MCHBAR32(0x70) & ~(3<<21)) | (1 << 21);
		else
			MCHBAR32(0x70) = (MCHBAR32(0x70) & ~(3<<21));
	}
	if (stepping > STEPPING_B2)
		MCHBAR32(0x44) |= 1 << 30;
	MCHBAR32(0x44) |= 1 << 31;
	if (sysinfo->cores == 2)
		MCHBAR32(0x44) |= 1 << 26;
	MCHBAR32(0x44) |= 1 << 21;
	MCHBAR32(0x44) = (MCHBAR32(0x44) & ~(3 << 24)) | (2 << 24);
	MCHBAR32(0x44) |= 1 << 5;
	MCHBAR32(0x44) |= 1 << 4;
	MCHBAR32(0x90) = (MCHBAR32(0x90) & ~7) | 4;
	MCHBAR32(0x94) |= 1 << 29;
	MCHBAR32(0x94) |= 1 << 11;
	if (stepping < STEPPING_B0)
		MCHBAR32(0x94) = (MCHBAR32(0x94) & ~(3 << 19)) | (2 << 19);
	if (stepping > STEPPING_B2)
		MCHBAR32(0x94) |= 1 << 21;
	MCHBAR8(0xb00) &= ~1;
	MCHBAR8(0xb00) |= 1 << 7;
	if (fsb != FSB_CLOCK_1067MHz)
		MCHBAR8(0x75) |= 1 << 6;
	else
		MCHBAR8(0x75) &= 1 << 1;
	MCHBAR8(0x77) |= 3;
	if (stepping >= STEPPING_B1)
		MCHBAR8(0x77) |= 1 << 2;
	if (stepping > STEPPING_B2)
		MCHBAR8(0x77) |= 1 << 4;
	if (MCHBAR16(0x90) & 0x100)
		MCHBAR8(0x90) &= ~(7 << 4);
	if (stepping >= STEPPING_B0)
		MCHBAR8(0xd0) |= 1 << 1;
	MCHBAR8(0xbd8) |= 3 << 2;
	if (stepping >= STEPPING_B3)
		MCHBAR32(0x70) |= 1 << 0;
	MCHBAR32(0x70) |= 1 << 3;
	if (stepping >= STEPPING_B0)
		MCHBAR32(0x70) &= ~(1 << 16);
	else
		MCHBAR32(0x70) |= 1 << 16;
	if (stepping >= STEPPING_B3)
		MCHBAR8(0xc14) |= 1 << 1;
	if (stepping >= STEPPING_B1)
		MCHBAR16(0xffc) = (MCHBAR16(0xffc) & ~0x7ff) | 0x7c0;
	MCHBAR16(0x48) = (MCHBAR16(0x48) & ~(0xff << 2)) | (0xaa << 2);
	if (stepping == STEPPING_CONVERSION_A1) {
		MCHBAR16(0x40) |= 1 << 12;
		MCHBAR32(0x94) |= 3 << 22;
	}

	const int cpu_supports_super_lfm = rdmsr(0xee).lo & (1 << 27);
	if ((stepping >= STEPPING_B0) && cpu_supports_super_lfm) {
		MCHBAR16(CLKCFG_MCHBAR) &= ~(1 << 7);
		MCHBAR16(CLKCFG_MCHBAR) |= 1 << 14;
	} else {
		MCHBAR16(CLKCFG_MCHBAR) &= ~(1 << 14);
		MCHBAR16(CLKCFG_MCHBAR) |= 1 << 7;
		MCHBAR32(0x44) &= ~(1 << 31); /* Was set above. */
	}

	if ((sysinfo->gfx_type != GMCH_PM45) &&
			(sysinfo->gfx_type != GMCH_UNKNOWN))
		init_freq_scaling(sysinfo->gfx_type,
				  sysinfo->gs45_low_power_mode);

	/* This has to be the last write to CLKCFG. */
	if ((fsb == FSB_CLOCK_1067MHz) && (memclk == MEM_CLOCK_667MT))
		MCHBAR32(CLKCFG_MCHBAR) &= ~(1 << 17);
}
