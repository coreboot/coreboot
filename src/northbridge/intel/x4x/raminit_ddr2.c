/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <pc80/mc146818rtc.h>
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_I82801GX)
#include <southbridge/intel/i82801gx/i82801gx.h>
#else
#include <southbridge/intel/i82801jx/i82801jx.h>
#endif
#include "iomap.h"
#include "x4x.h"

#define ME_UMA_SIZEMB 0

u32 fsb2mhz(u32 speed)
{
	return (speed * 267) + 800;
}

u32 ddr2mhz(u32 speed)
{
	static const u16 mhz[] = { 0, 0, 667, 800, 1067, 1333 };

	if (speed >= ARRAY_SIZE(mhz))
		return 0;

	return mhz[speed];
}

/* Find MSB bitfield location using bit scan reverse instruction */
static u8 msbpos(u32 val)
{
	u32 pos;

	if (val == 0) {
		printk(BIOS_WARNING, "WARNING: Input to BSR is zero\n");
		return 0;
	}

	asm ("bsrl %1, %0"
		: "=r"(pos)
		: "r"(val)
	);

	return (u8)(pos & 0xff);
}

static void clkcross_ddr2(struct sysinfo *s)
{
	u8 i, j;
	MCHBAR16(0xc1c) = MCHBAR16(0xc1c) | (1 << 15);

	static const u32 clkxtab[6][3][13] = {
		/* MEMCLK 400 N/A */
		{{}, {}, {} },
		/* MEMCLK 533 N/A */
		{{}, {}, {} },
		/* MEMCLK 667
		 * FSB 800 */
		{{0x1f1f1f1f, 0x1a07070b, 0x00000000, 0x10000000,
		  0x20010208, 0x04080000, 0x10010002, 0x00000000,
		  0x00000000, 0x02000000, 0x04000100, 0x08000000,
		  0x10200204},
		/* FSB 1067 */
		{0x6d5b1f1f, 0x0f0f0f0f, 0x00000000, 0x20000000,
		 0x80020410, 0x02040008, 0x10000100, 0x00000000,
		 0x00000000, 0x04000000, 0x08000102, 0x20000000,
		 0x40010208},
		/* FSB 1333 */
		{0x05050303, 0xffffffff, 0xffff0000, 0x00000000,
		 0x08020000, 0x00000000, 0x00020001, 0x00000000,
		 0x00000000, 0x00000000, 0x08010204, 0x00000000,
		 0x04010000} },
		/* MEMCLK 800
		 * FSB 800 */
		{{0xffffffff, 0x05030305, 0x0000ffff, 0x0000000,
		  0x08010204, 0x00000000, 0x08010204, 0x0000000,
		  0x00000000, 0x00000000, 0x00020001, 0x0000000,
		  0x04080102},
		/* FSB 1067 */
		{0x07070707, 0x06030303, 0x00000000, 0x00000000,
		 0x08010200, 0x00000000, 0x04000102, 0x00000000,
		 0x00000000, 0x00000000, 0x00020001, 0x00000000,
		 0x02040801},
		/* FSB 1333 */
		{0x0d0b0707, 0x3e1f1f2f, 0x01010000, 0x00000000,
		 0x10020400, 0x02000000, 0x00040100, 0x00000000,
		 0x00000000, 0x04080000, 0x00100102, 0x00000000,
		 0x08100200} },
		/* MEMCLK 1067 */
		{{},
		/* FSB 1067 */
		{0xffffffff, 0x05030305, 0x0000ffff, 0x00000000,
		 0x04080102, 0x00000000, 0x08010204, 0x00000000,
		 0x00000000, 0x00000000, 0x00020001, 0x00000000,
		 0x02040801},
		/* FSB 1333 */
		{0x0f0f0f0f, 0x5b1f1f6d, 0x00000000, 0x00000000,
		 0x08010204, 0x04000000, 0x00080102, 0x00000000,
		 0x00000000, 0x02000408, 0x00100001, 0x00000000,
		 0x04080102} },
		/* MEMCLK 1333 */
		{{}, {},
		/* FSB 1333 */
		{0xffffffff, 0x05030305, 0x0000ffff, 0x00000000,
		 0x04080102, 0x00000000, 0x04080102, 0x00000000,
		 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		 0x02040801} }
	};

	i = (u8)s->selected_timings.mem_clk;
	j = (u8)s->selected_timings.fsb_clk;

	MCHBAR32(0xc04) = clkxtab[i][j][0];
	MCHBAR32(0xc50) = clkxtab[i][j][1];
	MCHBAR32(0xc54) = clkxtab[i][j][2];
	MCHBAR8(0xc08) = MCHBAR8(0xc08) | (1 << 7);
	MCHBAR32(0x6d8) = clkxtab[i][j][3];
	MCHBAR32(0x6e0) = clkxtab[i][j][3];
	MCHBAR32(0x6dc) = clkxtab[i][j][4];
	MCHBAR32(0x6e4) = clkxtab[i][j][4];
	MCHBAR32(0x6e8) = clkxtab[i][j][5];
	MCHBAR32(0x6f0) = clkxtab[i][j][5];
	MCHBAR32(0x6ec) = clkxtab[i][j][6];
	MCHBAR32(0x6f4) = clkxtab[i][j][6];
	MCHBAR32(0x6f8) = clkxtab[i][j][7];
	MCHBAR32(0x6fc) = clkxtab[i][j][8];
	MCHBAR32(0x708) = clkxtab[i][j][11];
	MCHBAR32(0x70c) = clkxtab[i][j][12];
}

static void setioclk_ddr2(struct sysinfo *s)
{
	MCHBAR32(0x1bc) = 0x08060402;
	MCHBAR16(0x1c0) = MCHBAR16(0x1c0) | 0x200;
	MCHBAR16(0x1c0) = MCHBAR16(0x1c0) | 0x100;
	MCHBAR16(0x1c0) = MCHBAR16(0x1c0) | 0x20;
	MCHBAR16(0x1c0) = MCHBAR16(0x1c0) & ~1;
	switch (s->selected_timings.mem_clk) {
	default:
	case MEM_CLOCK_800MHz:
	case MEM_CLOCK_1066MHz:
		MCHBAR8(0x5d9) = (MCHBAR8(0x5d9) & ~0x2) | 0x2;
		MCHBAR8(0x9d9) = (MCHBAR8(0x9d9) & ~0x2) | 0x2;
		MCHBAR8(0x189) = (MCHBAR8(0x189) & ~0xf0) | 0xc0;
		MCHBAR8(0x189) = (MCHBAR8(0x189) & ~0xf0) | 0xe0;
		MCHBAR8(0x189) = (MCHBAR8(0x189) & ~0xf0) | 0xa0;
		break;
	case MEM_CLOCK_667MHz:
	case MEM_CLOCK_1333MHz:
		MCHBAR8(0x5d9) = MCHBAR8(0x5d9) & ~0x2;
		MCHBAR8(0x9d9) = MCHBAR8(0x9d9) & ~0x2;
		MCHBAR8(0x189) = (MCHBAR8(0x189) & ~0xf0) | 0x40;
		break;
	}
	MCHBAR32(0x594) = MCHBAR32(0x594) | (1 << 31);
	MCHBAR32(0x994) = MCHBAR32(0x994) | (1 << 31);
}

static void launch_ddr2(struct sysinfo *s)
{
	u8 i;
	u32 launch1 = 0x58001117;
	u32 launch2 = 0;
	u32 launch3 = 0;

	if (s->selected_timings.CAS == 5)
		launch2 = 0x00220201;
	else if (s->selected_timings.CAS == 6)
		launch2 = 0x00230302;
	else
		die("Unsupported CAS\n");

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		MCHBAR32(0x400*i + 0x220) = launch1;
		MCHBAR32(0x400*i + 0x224) = launch2;
		MCHBAR32(0x400*i + 0x21c) = launch3;
		MCHBAR32(0x400*i + 0x248) = MCHBAR32(0x400*i + 0x248) | (1 << 23);
	}

	MCHBAR32(0x2c0) = (MCHBAR32(0x2c0) & ~0x58000000) | 0x48000000;
	MCHBAR32(0x2c0) = MCHBAR32(0x2c0) | 0x1e0;
	MCHBAR32(0x2c4) = (MCHBAR32(0x2c4) & ~0xf) | 0xc;
}

static void clkset0(u8 ch, const struct dll_setting *setting)
{
	MCHBAR16(0x400*ch + 0x5a0) = (MCHBAR16(0x400*ch + 0x5a0) & ~0xc440) |
		(setting->clk_delay << 14) |
		(setting->db_sel << 6) |
		(setting->db_en << 10);
	MCHBAR8(0x400*ch + 0x581) = (MCHBAR8(0x400*ch + 0x581) & ~0x70) |
		(setting->pi << 4);
	MCHBAR8(0x400*ch + 0x581) = (MCHBAR8(0x400*ch + 0x581) & ~0xf) |
		setting->tap;
}

static void clkset1(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32(0x400*ch + 0x5a0) = (MCHBAR32(0x400*ch + 0x5a0) & ~0x30880) |
		(setting->clk_delay << 16) |
		(setting->db_sel << 7) |
		(setting->db_en << 11);
	MCHBAR8(0x400*ch + 0x582) = (MCHBAR8(0x400*ch + 0x582) & ~0x70) |
		(setting->pi << 4);
	MCHBAR8(0x400*ch + 0x582) = (MCHBAR8(0x400*ch + 0x582) & ~0xf) |
		setting->tap;
}

static void ctrlset0(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32(0x400*ch + 0x59c) = (MCHBAR32(0x400*ch + 0x59c) & ~0x3300000) |
		(setting->clk_delay << 24) |
		(setting->db_sel << 20) |
		(setting->db_en << 21);
	MCHBAR8(0x400*ch + 0x584) = (MCHBAR8(0x400*ch + 0x584) & ~0x70) |
		(setting->pi << 4);
	MCHBAR8(0x400*ch + 0x584) = (MCHBAR8(0x400*ch + 0x584) & ~0xf) |
		setting->tap;
}

static void ctrlset1(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32(0x400*ch + 0x59c) = (MCHBAR32(0x400*ch + 0x59c) & ~0x18c00000) |
		(setting->clk_delay << 27) |
		(setting->db_sel << 22) |
		(setting->db_en << 23);
	MCHBAR8(0x400*ch + 0x585) = (MCHBAR8(0x400*ch + 0x585) & ~0x70) |
		(setting->pi << 4);
	MCHBAR8(0x400*ch + 0x585) = (MCHBAR8(0x400*ch + 0x585) & ~0xf) |
		setting->tap;
}

static void ctrlset2(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32(0x400*ch + 0x598) = (MCHBAR32(0x400*ch + 0x598) & ~0x18c00000) |
		(setting->clk_delay << 14) |
		(setting->db_sel << 12) |
		(setting->db_en << 13);
	MCHBAR8(0x400*ch + 0x586) = (MCHBAR8(0x400*ch + 0x586) & ~0x70) |
		(setting->pi << 4);
	MCHBAR8(0x400*ch + 0x586) = (MCHBAR8(0x400*ch + 0x586) & ~0xf) |
		setting->tap;
}

static void ctrlset3(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32(0x400*ch + 0x598) = (MCHBAR32(0x400*ch + 0x598) & ~0x18c00000) |
		(setting->clk_delay << 10) |
		(setting->db_sel << 8) |
		(setting->db_en << 9);
	MCHBAR8(0x400*ch + 0x587) = (MCHBAR8(0x400*ch + 0x587) & ~0x70) |
		(setting->pi << 4);
	MCHBAR8(0x400*ch + 0x587) = (MCHBAR8(0x400*ch + 0x587) & ~0xf) |
		setting->tap;
}

static void cmdset(u8 ch, const struct dll_setting *setting)
{
	MCHBAR8(0x400*ch + 0x598) = (MCHBAR8(0x400*ch + 0x598) & ~0x30) |
		(setting->clk_delay << 4);
	MCHBAR8(0x400*ch + 0x594) = (MCHBAR8(0x400*ch + 0x594) & ~0x60) |
		(setting->db_sel << 5) |
		(setting->db_en << 6);
	MCHBAR8(0x400*ch + 0x580) = (MCHBAR8(0x400*ch + 0x580) & ~0x70) |
		(setting->pi << 4);
	MCHBAR8(0x400*ch + 0x580) = (MCHBAR8(0x400*ch + 0x580) & ~0xf) |
		setting->tap;
}

/**
 * All finer DQ and DQS DLL settings are set to the same value
 * for each rank in a channel, while coarse is common.
 */
static void dqsset(u8 ch, u8 lane, const struct dll_setting *setting)
{
	int rank;

	MCHBAR32(0x400 * ch + 0x5fc) = (MCHBAR32(0x400 * ch + 0x5fc)
			& ~(1 << (lane * 4 + 1)))
		| (setting->coarse << (lane * 4 + 1));

	for (rank = 0; rank < 4; rank++) {
		MCHBAR32(0x400 * ch + 0x5b4 + rank * 4) =
			(MCHBAR32(0x400 * ch + 0x5b4 + rank * 4)
				& ~(0x201 << lane))
			| (setting->db_en << (9 + lane))
			| (setting->db_sel << lane);

		MCHBAR32(0x400*ch + 0x5c8 + rank * 4) =
			(MCHBAR32(0x400 * ch + 0x5c8 + rank * 4)
				& ~(0x3 << (16 + lane * 2)))
			| (setting->clk_delay << (16+lane * 2));

		MCHBAR8(0x400*ch + 0x520 + lane * 4 + rank) =
			(MCHBAR8(0x400*ch + 0x520 + lane*4) & ~0x7f)
			| (setting->pi << 4)
			| setting->tap;
	}
}

static void dqset(u8 ch, u8 lane, const struct dll_setting *setting)
{
	int rank;
	MCHBAR32(0x400 * ch + 0x5fc) = (MCHBAR32(0x400 * ch + 0x5fc)
			& ~(1 << (lane * 4)))
		| (setting->coarse << (lane * 4));

	for (rank = 0; rank < 4; rank++) {
		MCHBAR32(0x400 * ch + 0x5a4 + rank * 4) =
			(MCHBAR32(0x400 * ch + 0x5a4 + rank * 4)
				& ~(0x201 << lane))
			| (setting->db_en << (9 + lane))
			| (setting->db_sel << lane);

		MCHBAR32(0x400 * ch + 0x5c8 + rank * 4) =
			(MCHBAR32(0x400 * ch + 0x5c8 + rank * 4)
				& ~(0x3 << (lane * 2)))
			| (setting->clk_delay << (2 * lane));

		MCHBAR8(0x400*ch + 0x500 + lane * 4 + rank) =
			(MCHBAR8(0x400 * ch + 0x500 + lane * 4 + rank) & ~0x7f)
			| (setting->pi << 4)
			| setting->tap;
	}
}

static void timings_ddr2(struct sysinfo *s)
{
	u8 i;
	u8 twl, ta1, ta2, ta3, ta4;
	u8 reg8;
	u8 flag1 = 0;
	u8 flag2 = 0;
	u16 reg16;
	u32 reg32;
	u16 ddr, fsb;
	u8 trpmod = 0;
	u8 bankmod = 1;
	u8 pagemod = 0;
	u8 adjusted_cas;

	adjusted_cas = s->selected_timings.CAS - 3;

	u16 fsb2ps[3] = {
		5000, // 800
		3750, // 1067
		3000  // 1333
	};

	u16 ddr2ps[6] = {
		5000, // 400
		3750, // 533
		3000, // 667
		2500, // 800
		1875, // 1067
		1500  // 1333
	};

	u16 lut1[6] = {
		0,
		0,
		2600,
		3120,
		4171,
		5200
	};

	ta1 = 6;
	ta2 = 6;
	ta3 = 5;
	ta4 = 8;

	twl = s->selected_timings.CAS - 1;

	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		if (s->dimms[i].n_banks == N_BANKS_8) {
			trpmod = 1;
			bankmod = 0;
		}
		if (s->dimms[i].page_size == 2048)
			pagemod = 1;
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		MCHBAR8(0x400*i + 0x26f) = MCHBAR8(0x400*i + 0x26f) | 0x3;
		MCHBAR8(0x400*i + 0x228) = (MCHBAR8(0x400*i + 0x228) & ~0x7) | 0x2;
		MCHBAR8(0x400*i + 0x240) = (MCHBAR8(0x400*i + 0x240) & ~0xf0)
		  | (0 << 4); /* tWL - x ?? */
		MCHBAR8(0x400*i + 0x240) = (MCHBAR8(0x400*i + 0x240) & ~0xf) |
			adjusted_cas;
		MCHBAR16(0x400*i + 0x265) = (MCHBAR16(0x400*i + 0x265) & ~0x3f00) |
			((adjusted_cas + 9) << 8);

		reg16 = (s->selected_timings.tRAS << 11) |
			((twl + 4 + s->selected_timings.tWR) << 6) |
			((2 + MAX(s->selected_timings.tRTP, 2)) << 2) | 1;
		MCHBAR16(0x400*i + 0x250) = reg16;

		reg32 = (bankmod << 21) |
			(s->selected_timings.tRRD << 17) |
			(s->selected_timings.tRP << 13) |
			((s->selected_timings.tRP + trpmod) << 9) |
			s->selected_timings.tRFC;
		reg8 = (MCHBAR8(0x400*i + 0x26f) >> 1) & 1;
		if (bankmod) {
			switch (s->selected_timings.mem_clk) {
			default:
			case MEM_CLOCK_667MHz:
				if (reg8) {
					if (pagemod)
						reg32 |= 16 << 22;
					else
						reg32 |= 12 << 22;
				} else {
					if (pagemod)
						reg32 |= 18 << 22;
					else
						reg32 |= 14 << 22;
				}
				break;
			case MEM_CLOCK_800MHz:
				if (reg8) {
					if (pagemod)
						reg32 |= 18 << 22;
					else
						reg32 |= 14 << 22;
				} else {
					if (pagemod)
						reg32 |= 20 << 22;
					else
						reg32 |= 16 << 22;
				}
				break;
			}
		}
		MCHBAR32(0x400*i + 0x252) = reg32;

		MCHBAR16(0x400*i + 0x256) = (s->selected_timings.tRCD << 12) |
			(0x4 << 8) | (ta2 << 4) | ta4;

		MCHBAR32(0x400*i + 0x258) = (s->selected_timings.tRCD << 17) |
			((twl + 4 + s->selected_timings.tWTR) << 12) |
			(ta3 << 8) | (4 << 4) | ta1;

		MCHBAR16(0x400*i + 0x25b) = ((s->selected_timings.tRP + trpmod) << 9) |
			s->selected_timings.tRFC;

		MCHBAR16(0x400*i + 0x260) = (MCHBAR16(0x400*i + 0x260) & ~0x3fe) | (100 << 1);
		MCHBAR8(0x400*i + 0x264) = 0xff;
		MCHBAR8(0x400*i + 0x25d) = (MCHBAR8(0x400*i + 0x25d) & ~0x3f) |
			s->selected_timings.tRAS;
		MCHBAR16(0x400*i + 0x244) = 0x2310;

		switch (s->selected_timings.mem_clk) {
		case MEM_CLOCK_667MHz:
			reg8 = 0;
			break;
		default:
			reg8 = 1;
			break;
		}

		MCHBAR8(0x400*i + 0x246) = (MCHBAR8(0x400*i + 0x246) & ~0x1f) |
			(reg8 << 2) | 1;

		fsb = fsb2ps[s->selected_timings.fsb_clk];
		ddr = ddr2ps[s->selected_timings.mem_clk];
		reg32 = (u32)((adjusted_cas + 7 + reg8) * ddr);
		reg32 = (u32)((reg32 / fsb) << 8);
		reg32 |= 0x0e000000;
		if ((fsb2mhz(s->selected_timings.fsb_clk) /
		     ddr2mhz(s->selected_timings.mem_clk)) > 2) {
			reg32 |= 1 << 24;
		}
		MCHBAR32(0x400*i + 0x248) = (MCHBAR32(0x400*i + 0x248) & ~0x0f001f00) |
			reg32;

		if (twl > 2)
			flag1 = 1;

		if (s->selected_timings.mem_clk >= MEM_CLOCK_800MHz)
			flag2 = 1;

		reg16 = (u8)(twl - 1 - flag1 - flag2);
		reg16 |= reg16 << 4;
		if (s->selected_timings.mem_clk == MEM_CLOCK_1333MHz) {
			if (reg16)
				reg16--;
		}
		reg16 |= flag1 << 8;
		reg16 |= flag2 << 9;
		MCHBAR16(0x400*i + 0x24d) = (MCHBAR16(0x400*i + 0x24d) & ~0x1ff) | reg16;
		MCHBAR16(0x400*i + 0x25e) = 0x15a5;
		MCHBAR32(0x400*i + 0x265) = MCHBAR32(0x400*i + 0x265) & ~0x1f;
		MCHBAR32(0x400*i + 0x269) = (MCHBAR32(0x400*i + 0x269) & ~0x000fffff) |
			(0x3f << 14) | lut1[s->selected_timings.mem_clk];
		MCHBAR8(0x400*i + 0x274) = MCHBAR8(0x400*i + 0x274) | 1;
		MCHBAR8(0x400*i + 0x24c) = MCHBAR8(0x400*i + 0x24c) & ~0x3;

		reg16 = 0;
		switch (s->selected_timings.mem_clk) {
		default:
		case MEM_CLOCK_667MHz:
			reg16 = 0x99;
			break;
		case MEM_CLOCK_800MHz:
			if (s->selected_timings.CAS == 5)
				reg16 = 0x19a;
			else if (s->selected_timings.CAS == 6)
				reg16 = 0x9a;
			break;
		}
		reg16 &= 0x7;
		reg16 += twl + 9;
		reg16 <<= 10;
		MCHBAR16(0x400*i + 0x24d) = (MCHBAR16(0x400*i + 0x24d) & ~0x7c00) | reg16;
		MCHBAR8(0x400*i + 0x267) = (MCHBAR8(0x400*i + 0x267) & ~0x3f) | 0x13;
		MCHBAR8(0x400*i + 0x268) = (MCHBAR8(0x400*i + 0x268) & ~0xff) | 0x4a;

		reg16 = (MCHBAR16(0x400*i + 0x269) & 0xc000) >> 2;
		reg16 += 2 << 12;
		reg16 |= (0x15 << 6) | 0x1f;
		MCHBAR16(0x400*i + 0x26d) = (MCHBAR16(0x400*i + 0x26d) & ~0x7fff) | reg16;

		reg32 = (1 << 25) | (6 << 27);
		MCHBAR32(0x400*i + 0x269) = (MCHBAR32(0x400*i + 0x269) & ~0xfa300000) | reg32;
		MCHBAR8(0x400*i + 0x271) = MCHBAR8(0x400*i + 0x271) & ~0x80;
		MCHBAR8(0x400*i + 0x274) = MCHBAR8(0x400*i + 0x274) & ~0x6;
	} // END EACH POPULATED CHANNEL

	reg16 = 0x1f << 5;
	reg16 |= 0xe << 10;
	MCHBAR16(0x125) = (MCHBAR16(0x125) & ~0x3fe0) | reg16;
	MCHBAR16(0x127) = (MCHBAR16(0x127) & ~0x7ff) | 0x540;
	MCHBAR8(0x129) = MCHBAR8(0x129) | 0x1f;
	MCHBAR8(0x12c) = MCHBAR8(0x12c) | 0xa0;
	MCHBAR32(0x241) = (MCHBAR32(0x241) & ~0x1ffff) | 0x11;
	MCHBAR32(0x641) = (MCHBAR32(0x641) & ~0x1ffff) | 0x11;
	MCHBAR8(0x246) = MCHBAR8(0x246) & ~0x10;
	MCHBAR8(0x646) = MCHBAR8(0x646) & ~0x10;
	MCHBAR32(0x120) = (2 << 29) | (1 << 28) | (1 << 23) | 0xd7f5f;
	reg8 = (u8)((MCHBAR32(0x252) & 0x1e000) >> 13);
	MCHBAR8(0x12d) = (MCHBAR8(0x12d) & ~0xf0) | (reg8 << 4);
	reg8 = (u8)((MCHBAR32(0x258) & 0x1e0000) >> 17);
	MCHBAR8(0x12d) = (MCHBAR8(0x12d) & ~0xf) | reg8;
	MCHBAR8(0x12f) = 0x4c;
	reg32 = (1 << 31) | (0x80 << 14) | (1 << 13) | (0xa << 9);
	MCHBAR32(0x6c0) = (MCHBAR32(0x6c0) & ~0xffffff00) | reg32;
	MCHBAR8(0x6c4) = (MCHBAR8(0x6c4) & ~0x7) | 0x2;
}

static void dll_ddr2(struct sysinfo *s)
{
	u8 i, j, r, reg8, clk, async = 0;
	u16 reg16 = 0;
	u32 reg32 = 0;
	u8 lane;

	MCHBAR16(0x180) = (MCHBAR16(0x180) & ~0x7e06) | 0xc04;
	MCHBAR16(0x182) = (MCHBAR16(0x182) & ~0x3ff) | 0xc8;
	MCHBAR16(0x18a) = (MCHBAR16(0x18a) & ~0x1f1f) | 0x0f0f;
	MCHBAR16(0x1b4) = (MCHBAR16(0x1b4) & ~0x8020) | 0x100;
	MCHBAR8(0x194) = (MCHBAR8(0x194) & ~0x77) | 0x33;
	switch (s->selected_timings.mem_clk) {
	default:
	case MEM_CLOCK_667MHz:
		reg16 = (0xa << 9) | 0xa;
		break;
	case MEM_CLOCK_800MHz:
		reg16 = (0x9 << 9) | 0x9;
		break;
	}
	MCHBAR16(0x19c) = (MCHBAR16(0x19c) & ~0x1e0f) | reg16;
	MCHBAR16(0x19c) = (MCHBAR16(0x19c) & ~0x2030) | 0x2010;
	udelay(1);
	MCHBAR16(0x198) = MCHBAR16(0x198) & ~0x100;

	MCHBAR16(0x1c8) = (MCHBAR16(0x1c8) & ~0x1f) | 0xd;

	udelay(1);
	MCHBAR8(0x190) = MCHBAR8(0x190) & ~1;
	udelay(1); // 533ns
	MCHBAR32(0x198) = MCHBAR32(0x198) & ~0x11554000;
	udelay(1);
	MCHBAR32(0x198) = MCHBAR32(0x198) & ~0x1455;
	udelay(1);
	MCHBAR8(0x583) = MCHBAR8(0x583) & ~0x1c;
	MCHBAR8(0x983) = MCHBAR8(0x983) & ~0x1c;
	udelay(1); // 533ns
	MCHBAR8(0x583) = MCHBAR8(0x583) & ~0x3;
	MCHBAR8(0x983) = MCHBAR8(0x983) & ~0x3;
	udelay(1); // 533ns

	// ME related
	MCHBAR32(0x1a0) = (MCHBAR32(0x1a0) & ~0x7ffffff) | 0x551803;

	MCHBAR16(0x1b4) = MCHBAR16(0x1b4) & ~0x800;
	MCHBAR8(0x1a8) = MCHBAR8(0x1a8) | 0xf0;

	FOR_EACH_CHANNEL(i) {
		reg16 = 0;
		MCHBAR16(0x400*i + 0x59c) = MCHBAR16(0x400*i + 0x59c) & ~0x3000;

		reg32 = 0;
		FOR_EACH_RANK_IN_CHANNEL(r) {
			if (!RANK_IS_POPULATED(s->dimms, i, r))
				reg32 |= 0x111 << r;
		}

		MCHBAR32(0x400*i + 0x59c) = (MCHBAR32(0x400*i + 0x59c) & ~0xfff) | reg32;
		MCHBAR8(0x400*i + 0x594) = MCHBAR8(0x400*i + 0x594) & ~1;

		if (!CHANNEL_IS_POPULATED(s->dimms, i)) {
			printk(BIOS_DEBUG, "No dimms in channel %d\n", i);
			reg8 = 0x3f;
		} else if (ONLY_DIMMA_IS_POPULATED(s->dimms, i)) {
			printk(BIOS_DEBUG, "DimmA populated only in channel %d\n", i);
			reg8 = 0x38;
		} else if (ONLY_DIMMB_IS_POPULATED(s->dimms, i)) {
			printk(BIOS_DEBUG, "DimmB populated only in channel %d\n", i);
			reg8 =  0x7;
		} else if (BOTH_DIMMS_ARE_POPULATED(s->dimms, i)) {
			printk(BIOS_DEBUG, "Both dimms populated in channel %d\n", i);
			reg8 = 0;
		} else {
			die("Unhandled case\n");
		}

		//reg8 = 0x00; // FIXME don't switch on all clocks anyway

		MCHBAR32(0x400*i + 0x5a0) = (MCHBAR32(0x400*i + 0x5a0) & ~0x3f000000) |
			((u32)(reg8 << 24));
	} // END EACH CHANNEL

	MCHBAR8(0x1a8) = MCHBAR8(0x1a8) | 1;
	MCHBAR8(0x1a8) = MCHBAR8(0x1a8) & ~0x4;

	// Update DLL timing
	MCHBAR8(0x1a4) = MCHBAR8(0x1a4) & ~0x80;
	MCHBAR8(0x1a4) = MCHBAR8(0x1a4) | 0x40;
	MCHBAR16(0x5f0) = (MCHBAR16(0x5f0) & ~0x400) | 0x400;

	static const struct dll_setting dll_setting_667[23] = {
	//	tap  pi db  delay
		{13, 0, 1, 0, 0},
		{4,  1, 0, 0, 0},
		{13, 0, 1, 0, 0},
		{4,  5, 0, 0, 0},
		{4,  1, 0, 0, 0},
		{4,  1, 0, 0, 0},
		{4,  1, 0, 0, 0},
		{1,  5, 1, 1, 1},
		{1,  6, 1, 1, 1},
		{2,  0, 1, 1, 1},
		{2,  1, 1, 1, 1},
		{2,  1, 1, 1, 1},
		{14, 6, 1, 0, 0},
		{14, 3, 1, 0, 0},
		{14, 0, 1, 0, 0},
		{9,  0, 0, 0, 1},
		{9,  1, 0, 0, 1},
		{9,  2, 0, 0, 1},
		{9,  2, 0, 0, 1},
		{9,  1, 0, 0, 1},
		{6,  4, 0, 0, 1},
		{6,  2, 0, 0, 1},
		{5,  4, 0, 0, 1}
	};

	static const struct dll_setting dll_setting_800[23] = {
	//	tap  pi db  delay
		{11, 5, 1, 0, 0},
		{0,  5, 1, 1, 0},
		{11, 5, 1, 0, 0},
		{1,  4, 1, 1, 0},
		{0,  5, 1, 1, 0},
		{0,  5, 1, 1, 0},
		{0,  5, 1, 1, 0},
		{2,  5, 1, 1, 1},
		{2,  6, 1, 1, 1},
		{3,  0, 1, 1, 1},
		{3,  0, 1, 1, 1},
		{3,  3, 1, 1, 1},
		{2,  0, 1, 1, 1},
		{1,  3, 1, 1, 1},
		{0,  3, 1, 1, 1},
		{9,  3, 0, 0, 1},
		{9,  4, 0, 0, 1},
		{9,  5, 0, 0, 1},
		{9,  6, 0, 0, 1},
		{10, 0, 0, 0, 1},
		{8,  1, 0, 0, 1},
		{7,  5, 0, 0, 1},
		{6,  2, 0, 0, 1}
	};

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		MCHBAR16(0x400*i + 0x5f0) = (MCHBAR16(0x400*i + 0x5f0) & ~0x3fc) | 0x3fc;
		MCHBAR32(0x400*i + 0x5fc) = MCHBAR32(0x400*i + 0x5fc) & ~0xcccccccc;
		MCHBAR8(0x400*i + 0x5d9) = (MCHBAR8(0x400*i + 0x5d9) & ~0xf0) | 0x70;
		MCHBAR16(0x400*i + 0x590) = (MCHBAR16(0x400*i + 0x590) & ~0xffff) | 0x5555;
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		const struct dll_setting *setting;

		if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz)
			setting = dll_setting_667;
		else
			setting = dll_setting_800;

		clkset0(i, &setting[CLKSET0]);
		clkset1(i, &setting[CLKSET1]);
		ctrlset0(i, &setting[CTRL0]);
		ctrlset1(i, &setting[CTRL1]);
		ctrlset2(i, &setting[CTRL2]);
		ctrlset3(i, &setting[CTRL3]);
		cmdset(i, &setting[CMD]);
	}

	// XXX if not async mode
	MCHBAR16(0x180) = MCHBAR16(0x180) & ~0x8200;
	MCHBAR8(0x180) = MCHBAR8(0x180) | 0x4;
	j = 0;
	for (i = 0; i < 16; i++) {
		MCHBAR8(0x1c8) = (MCHBAR8(0x1c8) & ~0x1f) | i;
		MCHBAR8(0x180) = MCHBAR8(0x180) | 0x10;
		while (MCHBAR8(0x180) & 0x10)
			;
		if (MCHBAR32(0x184) == 0xffffffff) {
			j++;
			if (j >= 2)
				break;

			if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz) {
				j = 2;
				break;
			}
		} else {
			j = 0;
		}
	}
	if (i == 1 || ((i == 0) && s->selected_timings.mem_clk == MEM_CLOCK_667MHz)) {
		j = 0;
		i++;
		for (; i < 16; i++) {
			MCHBAR8(0x1c8) = (MCHBAR8(0x1c8) & ~0x1f) | i;
			MCHBAR8(0x180) = MCHBAR8(0x180) | 0x4;
			while (MCHBAR8(0x180) & 0x10)
				;
			if (MCHBAR32(0x184) == 0) {
				i++;
				break;
			}
		}
		for (; i < 16; i++) {
			MCHBAR8(0x1c8) = (MCHBAR8(0x1c8) & ~0x1f) | i;
			MCHBAR8(0x180) = MCHBAR8(0x180) | 0x10;
			while (MCHBAR8(0x180) & 0x10)
				;
			if (MCHBAR32(0x184) == 0xffffffff) {
				j++;
				if (j >= 2)
					break;
			} else {
				j = 0;
			}
		}
		if (j < 2) {
			MCHBAR8(0x1c8) = MCHBAR8(0x1c8) & ~0x1f;
			MCHBAR8(0x180) = MCHBAR8(0x180) | 0x10;
			while (MCHBAR8(0x180) & 0x10)
				;
			j = 2;
		}
	}

	if (j < 2) {
		MCHBAR8(0x1c8) = MCHBAR8(0x1c8) & ~0x1f;
		async = 1;
	}

	clk = 0x1a;
	if (async != 1) {
		reg8 = MCHBAR8(0x188) & 0x1e;
		if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz &&
			s->selected_timings.fsb_clk == FSB_CLOCK_800MHz) {
			clk = 0x10;
		} else if (s->selected_timings.mem_clk == MEM_CLOCK_800MHz) {
			clk = 0x10;
		} else {
			clk = 0x1a;
		}
	}
	MCHBAR8(0x180) = MCHBAR8(0x180) & ~0x80;

	if ((s->selected_timings.fsb_clk == FSB_CLOCK_800MHz) &&
	    (s->selected_timings.mem_clk == MEM_CLOCK_667MHz)) {
		i = MCHBAR8(0x1c8) & 0xf;
		i = (i + 10) % 14;
		MCHBAR8(0x1c8) = (MCHBAR8(0x1c8) & ~0x1f) | i;
		MCHBAR8(0x180) = MCHBAR8(0x180) | 0x10;
		while (MCHBAR8(0x180) & 0x10)
			;
	}

	reg8 = MCHBAR8(0x188) & ~1;
	MCHBAR8(0x188) = reg8;
	reg8 &= ~0x3e;
	reg8 |= clk;
	MCHBAR8(0x188) = reg8;
	reg8 |= 1;
	MCHBAR8(0x188) = reg8;

	if (s->selected_timings.mem_clk == MEM_CLOCK_1333MHz)
		MCHBAR8(0x18c) = MCHBAR8(0x18c) | 1;

	// Program DQ/DQS dll settings
	reg32 = 0;
	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		for (lane = 0; lane < 8; lane++) {
			if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz)
				reg32 = 0x06db7777;
			else if (s->selected_timings.mem_clk == MEM_CLOCK_800MHz)
				reg32 = 0x00007777;
			MCHBAR32(0x400*i + 0x540 + lane*4) =
				(MCHBAR32(0x400*i + 0x540 + lane*4) & 0x0fffffff) |
				reg32;
		}
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz) {
			for (lane = 0; lane < 8; lane++)
				dqsset(i, lane, &dll_setting_667[DQS1+lane]);
			for (lane = 0; lane < 8; lane++)
				dqset(i, lane, &dll_setting_667[DQ1+lane]);
		} else {
			for (lane = 0; lane < 8; lane++)
				dqsset(i, lane, &dll_setting_800[DQS1+lane]);
			for (lane = 0; lane < 8; lane++)
				dqset(i, lane, &dll_setting_800[DQ1+lane]);
		}
	}
}

static void rcomp_ddr2(struct sysinfo *s)
{
	u8 i, j, k;
	u32 x32a[8] = { 0x04040404, 0x06050505, 0x09090807, 0x0D0C0B0A,
			0x04040404, 0x08070605, 0x0C0B0A09, 0x100F0E0D };
	u16 x378[6] = { 0, 0xAAAA, 0x7777, 0x7777, 0x7777, 0x7777 };
	u32 x382[6] = { 0, 0x02020202, 0x02020202, 0x02020202, 0x04030303, 0x04030303 };
	u32 x386[6] = { 0, 0x03020202, 0x03020202, 0x03020202, 0x05040404, 0x05040404 };
	u32 x38a[6] = { 0, 0x04040303, 0x04040303, 0x04040303, 0x07070605, 0x07070605 };
	u32 x38e[6] = { 0, 0x06060505, 0x06060505, 0x06060505, 0x09090808, 0x09090808 };
	u32 x392[6] = { 0, 0x02020202, 0x02020202, 0x02020202, 0x03030202, 0x03030202 };
	u32 x396[6] = { 0, 0x03030202, 0x03030202, 0x03030202, 0x05040303, 0x05040303 };
	u32 x39a[6] = { 0, 0x04040403, 0x04040403, 0x04040403, 0x07070605, 0x07070605 };
	u32 x39e[6] = { 0, 0x06060505, 0x06060505, 0x06060505, 0x08080808, 0x08080808 };
	u16 addr[6] = { 0x31c, 0x374, 0x3a2, 0x3d0, 0x3fe, 0x42c };
	u8 bit[6] = { 0, 0, 1, 1, 0, 0 };

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		for (j = 0; j < 6; j++) {
			if (j == 0) {
				MCHBAR32(0x400*i + addr[j]) =
					(MCHBAR32(0x400*i + addr[j]) & ~0xff000) | 0xaa000;
				MCHBAR16(0x400*i + 0x320) = (MCHBAR16(0x400*i + 0x320) & ~0xffff) | 0x6666;
				for (k = 0; k < 8; k++) {
					MCHBAR32(0x400*i + addr[j] + 0xe + (k << 2)) =
						(MCHBAR32(0x400*i + addr[j] + 0xe + (k << 2)) & ~0x3f3f3f3f) | x32a[k];
					MCHBAR32(0x400*i + addr[j] + 0x2e + (k << 2)) =
						(MCHBAR32(0x400*i + addr[j] + 0x2e + (k << 2)) & ~0x3f3f3f3f) | x32a[k];
				}
			} else {
				MCHBAR16(0x400*i + addr[j]) = (MCHBAR16(0x400*i + addr[j]) & ~0xf000) | 0xa000;
				MCHBAR16(0x400*i + addr[j] + 4) = (MCHBAR16(0x400*i + addr[j] + 4) & ~0xffff) |
					x378[j];
				MCHBAR32(0x400*i + addr[j] + 0xe) =
					(MCHBAR32(0x400*i + addr[j] + 0xe) & ~0x3f3f3f3f) | x382[j];
				MCHBAR32(0x400*i + addr[j] + 0x12) =
					(MCHBAR32(0x400*i + addr[j] + 0x12) & ~0x3f3f3f3f) | x386[j];
				MCHBAR32(0x400*i + addr[j] + 0x16) =
					(MCHBAR32(0x400*i + addr[j] + 0x16) & ~0x3f3f3f3f) | x38a[j];
				MCHBAR32(0x400*i + addr[j] + 0x1a) =
					(MCHBAR32(0x400*i + addr[j] + 0x1a) & ~0x3f3f3f3f) | x38e[j];
				MCHBAR32(0x400*i + addr[j] + 0x1e) =
					(MCHBAR32(0x400*i + addr[j] + 0x1e) & ~0x3f3f3f3f) | x392[j];
				MCHBAR32(0x400*i + addr[j] + 0x22) =
					(MCHBAR32(0x400*i + addr[j] + 0x22) & ~0x3f3f3f3f) | x396[j];
				MCHBAR32(0x400*i + addr[j] + 0x26) =
					(MCHBAR32(0x400*i + addr[j] + 0x26) & ~0x3f3f3f3f) | x39a[j];
				MCHBAR32(0x400*i + addr[j] + 0x2a) =
					(MCHBAR32(0x400*i + addr[j] + 0x2a) & ~0x3f3f3f3f) | x39e[j];
			}
			MCHBAR8(0x400*i + addr[j]) = (MCHBAR8(0x400*i + addr[j]) & ~1) | bit[j];
		}
		MCHBAR8(0x400*i + 0x45a) = (MCHBAR8(0x400*i + 0x45a) & ~0x3f) | 0x12;
		MCHBAR8(0x400*i + 0x45e) = (MCHBAR8(0x400*i + 0x45e) & ~0x3f) | 0x12;
		MCHBAR8(0x400*i + 0x462) = (MCHBAR8(0x400*i + 0x462) & ~0x3f) | 0x12;
		MCHBAR8(0x400*i + 0x466) = (MCHBAR8(0x400*i + 0x466) & ~0x3f) | 0x12;
	} // END EACH POPULATED CHANNEL

	MCHBAR32(0x134) = (MCHBAR32(0x134) & ~0x63c00) | 0x63c00;
	MCHBAR16(0x174) = (MCHBAR16(0x174) & ~0x63ff) | 0x63ff;
	MCHBAR16(0x178) = 0x0135;
	MCHBAR32(0x130) = (MCHBAR32(0x130) & ~0x7bdffe0) | 0x7a9ffa0;

	if (!CHANNEL_IS_POPULATED(s->dimms, 0))
		MCHBAR32(0x130) = MCHBAR32(0x130) & ~(1 << 27);
	if (!CHANNEL_IS_POPULATED(s->dimms, 1))
		MCHBAR32(0x130) = MCHBAR32(0x130) & ~(1 << 28);

	MCHBAR8(0x130) = MCHBAR8(0x130) | 1;
}

static void odt_ddr2(struct sysinfo *s)
{
	u8 i;
	u16 odt[16][2] = {
		{ 0x0000, 0x0000 }, // NC_NC
		{ 0x0000, 0x0001 }, // x8SS_NC
		{ 0x0000, 0x0011 }, // x8DS_NC
		{ 0x0000, 0x0001 }, // x16SS_NC
		{ 0x0004, 0x0000 }, // NC_x8SS
		{ 0x0101, 0x0404 }, // x8SS_x8SS
		{ 0x0101, 0x4444 }, // x8DS_x8SS
		{ 0x0101, 0x0404 }, // x16SS_x8SS
		{ 0x0044, 0x0000 }, // NC_x8DS
		{ 0x1111, 0x0404 }, // x8SS_x8DS
		{ 0x1111, 0x4444 }, // x8DS_x8DS
		{ 0x1111, 0x0404 }, // x16SS_x8DS
		{ 0x0004, 0x0000 }, // NC_x16SS
		{ 0x0101, 0x0404 }, // x8SS_x16SS
		{ 0x0101, 0x4444 }, // x8DS_x16SS
		{ 0x0101, 0x0404 }, // x16SS_x16SS
	};

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		MCHBAR16(0x400*i + 0x298) = odt[s->dimm_config[i]][1];
		MCHBAR16(0x400*i + 0x294) = odt[s->dimm_config[i]][0];
		MCHBAR16(0x400*i + 0x29c) = (MCHBAR16(0x400*i + 0x29c) & ~0xfff) | 0x66b;
		MCHBAR32(0x400*i + 0x260) = (MCHBAR32(0x400*i + 0x260) & ~0x70e3c00) | 0x3063c00;
	}
}

static void dojedec_ddr2(u8 r, u8 ch, u8 cmd, u16 val)
{
	u32 addr = (ch << 29) | (r*0x08000000);
	volatile u32 rubbish;

	MCHBAR8(0x271) = (MCHBAR8(0x271) & ~0x3e) | cmd;
	MCHBAR8(0x671) = (MCHBAR8(0x671) & ~0x3e) | cmd;
	rubbish = read32((void *)((val<<3) | addr));
	udelay(10);
	MCHBAR8(0x271) = (MCHBAR8(0x271) & ~0x3e) | NORMALOP_CMD;
	MCHBAR8(0x671) = (MCHBAR8(0x671) & ~0x3e) | NORMALOP_CMD;
}

static void jedec_ddr2(struct sysinfo *s)
{
	u8 i;
	u16 mrsval, ch, r, v;

	u8 odt[16][4] = {
		{0x00, 0x00, 0x00, 0x00},
		{0x01, 0x00, 0x00, 0x00},
		{0x01, 0x01, 0x00, 0x00},
		{0x01, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x01, 0x00},
		{0x11, 0x00, 0x11, 0x00},
		{0x11, 0x11, 0x11, 0x00},
		{0x11, 0x00, 0x11, 0x00},
		{0x00, 0x00, 0x01, 0x01},
		{0x11, 0x00, 0x11, 0x11},
		{0x11, 0x11, 0x11, 0x11},
		{0x11, 0x00, 0x11, 0x11},
		{0x00, 0x00, 0x01, 0x00},
		{0x11, 0x00, 0x11, 0x00},
		{0x11, 0x11, 0x11, 0x00},
		{0x11, 0x00, 0x11, 0x00}
	};

	u16 jedec[12][2] = {
		{NOP_CMD, 0x0},
		{PRECHARGE_CMD, 0x0},
		{EMRS2_CMD, 0x0},
		{EMRS3_CMD, 0x0},
		{EMRS1_CMD, 0x0},
		{MRS_CMD, 0x100},	// DLL Reset
		{PRECHARGE_CMD, 0x0},
		{CBR_CMD, 0x0},
		{CBR_CMD, 0x0},
		{MRS_CMD, 0x0},		// DLL out of reset
		{EMRS1_CMD, 0x380},	// OCD calib default
		{EMRS1_CMD, 0x0}
	};

	mrsval = (s->selected_timings.CAS << 4) | ((s->selected_timings.tWR - 1) << 9) | 0xb;

	printk(BIOS_DEBUG, "MRS...\n");

	udelay(200);

	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		printk(BIOS_DEBUG, "CH%d: Found Rank %d\n", ch, r);
		for (i = 0; i < 12; i++) {
			v = jedec[i][1];
			switch (jedec[i][0]) {
			case EMRS1_CMD:
				v |= (odt[s->dimm_config[ch]][r] << 2);
				break;
			case MRS_CMD:
				v |= mrsval;
				break;
			default:
				break;
			}
			dojedec_ddr2(r + ch*4, ch, jedec[i][0], v);
			udelay(1);
			printk(RAM_SPEW, "Jedec step %d\n", i);
		}
	}
	printk(BIOS_DEBUG, "MRS done\n");
}

static void sdram_save_receive_enable(void)
{
	int i = 0;
	u16 reg16;
	u8 values[18];
	u8 lane, ch;

	FOR_EACH_CHANNEL(ch) {
		lane = 0;
		while (lane < 8) {
			values[i] = (MCHBAR8(0x400*ch + 0x560 + lane++ * 4) & 0xf);
			values[i++] |= (MCHBAR8(0x400*ch + 0x560 + lane++ * 4) & 0xf) << 4;
		}
		values[i++] = (MCHBAR32(0x400*ch + 0x248) >> 16) & 0xf;
		reg16 = MCHBAR16(0x400*ch + 0x5fa);
		values[i++] = reg16 & 0xff;
		values[i++] = (reg16 >> 8) & 0xff;
		reg16 = MCHBAR16(0x400*ch + 0x58c);
		values[i++] = reg16 & 0xff;
		values[i++] = (reg16 >> 8) & 0xff;
	}

	for (i = 0; i < ARRAY_SIZE(values); i++)
		cmos_write(values[i], 128 + i);
}

static void sdram_recover_receive_enable(void)
{
	u8 i;
	u32 reg32;
	u16 reg16;
	u8 values[18];
	u8 ch, lane;

	for (i = 0; i < ARRAY_SIZE(values); i++)
		values[i] = cmos_read(128 + i);

	i = 0;
	FOR_EACH_CHANNEL(ch) {
		lane = 0;
		while (lane < 8) {
			MCHBAR8(0x400*ch + 0x560 + lane++ * 4) = 0x70 |
				(values[i] & 0xf);
			MCHBAR8(0x400*ch + 0x560 + lane++ * 4) = 0x70 |
				((values[i++] >> 4) & 0xf);
		}
		reg32 = (MCHBAR32(0x400*ch + 0x248) & ~0xf0000)
		  | ((values[i++] & 0xf) << 16);
		MCHBAR32(0x400*ch + 0x248) = reg32;
		reg16 = values[i++];
		reg16 |= values[i++] << 8;
		MCHBAR16(0x400*ch + 0x5fa) = reg16;
		reg16 = values[i++];
		reg16 |= values[i++] << 8;
		MCHBAR16(0x400*ch + 0x58c) = reg16;
	}
}

static void sdram_program_receive_enable(struct sysinfo *s)
{
	/* enable upper CMOS */
	RCBA32(0x3400) = (1 << 2);

	/* Program Receive Enable Timings */
	if ((s->boot_path == BOOT_PATH_WARM_RESET)
		|| (s->boot_path == BOOT_PATH_RESUME)) {
		sdram_recover_receive_enable();
	} else {
		rcven(s);
		sdram_save_receive_enable();
	}
}

static void dradrb_ddr2(struct sysinfo *s)
{
	u8 map, i, ch, r, rankpop0, rankpop1;
	u32 c0dra = 0;
	u32 c1dra = 0;
	u32 c0drb = 0;
	u32 c1drb = 0;
	u32 dra;
	u32 dra0;
	u32 dra1;
	u16 totalmemorymb;
	u32 size, offset;
	u32 size0, size1;
	u8 dratab[2][2][2][4] = {
	{
		{
			{0xff, 0xff, 0xff, 0xff},
			{0xff, 0x00, 0x02, 0xff}
		},
		{
			{0xff, 0x01, 0xff, 0xff},
			{0xff, 0x03, 0xff, 0xff}
		}
	},
	{
		{
			{0xff, 0xff, 0xff, 0xff},
			{0xff, 0x04, 0x06, 0x08}
		},
		{
			{0xff, 0xff, 0xff, 0xff},
			{0x05, 0x07, 0x09, 0xff}
		}
	}
	};

	u8 drbtab[10] = {0x04, 0x02, 0x08, 0x04, 0x08, 0x04, 0x10, 0x08, 0x20, 0x10};

	// DRA
	rankpop0 = 0;
	rankpop1 = 0;
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		if (s->dimms[ch<<1].card_type != RAW_CARD_UNPOPULATED
				&& (r) < s->dimms[ch<<1].ranks)
			i = ch << 1;
		else
			i = (ch << 1) + 1;

		dra = dratab[s->dimms[i].n_banks]
			[s->dimms[i].width]
			[s->dimms[i].cols-9]
			[s->dimms[i].rows-12];
		if (s->dimms[i].n_banks == N_BANKS_8)
			dra |= 0x80;
		if (ch == 0) {
			c0dra |= dra << (r*8);
			rankpop0 |= 1 << r;
		} else {
			c1dra |= dra << (r*8);
			rankpop1 |= 1 << r;
		}
	}
	MCHBAR32(0x208) = c0dra;
	MCHBAR32(0x608) = c1dra;

	MCHBAR8(0x262) = (MCHBAR8(0x262) & ~0xf0) | ((rankpop0 << 4) & 0xf0);
	MCHBAR8(0x662) = (MCHBAR8(0x662) & ~0xf0) | ((rankpop1 << 4) & 0xf0);

	if (ONLY_DIMMA_IS_POPULATED(s->dimms, 0) ||
			ONLY_DIMMB_IS_POPULATED(s->dimms, 0))
		MCHBAR8(0x260) = MCHBAR8(0x260) | 1;
	if (ONLY_DIMMA_IS_POPULATED(s->dimms, 1) ||
			ONLY_DIMMB_IS_POPULATED(s->dimms, 1))
		MCHBAR8(0x660) = MCHBAR8(0x660) | 1;

	// DRB
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		if (s->dimms[ch<<1].card_type != RAW_CARD_UNPOPULATED
				&& (r) < s->dimms[ch<<1].ranks)
			i = ch << 1;
		else
			i = (ch << 1) + 1;
		if (ch == 0) {
			dra0 = (c0dra >> (8*r)) & 0x7f;
			c0drb = (u16)(c0drb + drbtab[dra0]);
			MCHBAR16(0x200 + 2*r) = c0drb;
		} else {
			dra1 = (c1dra >> (8*r)) & 0x7f;
			c1drb = (u16)(c1drb + drbtab[dra1]);
			MCHBAR16(0x600 + 2*r) = c1drb;
		}
	}

	s->channel_capacity[0] = c0drb << 6;
	s->channel_capacity[1] = c1drb << 6;
	totalmemorymb = s->channel_capacity[0] + s->channel_capacity[1];
	printk(BIOS_DEBUG, "Total memory: %d + %d = %dMiB\n",
		s->channel_capacity[0], s->channel_capacity[1], totalmemorymb);

	rankpop1 >>= 4;
	if (rankpop1) {
		MCHBAR16(0x600 + 2*msbpos(rankpop1)) = c0drb + c1drb;
		MCHBAR16(0x602 + 2*msbpos(rankpop1)) = c0drb + c1drb;
		MCHBAR16(0x604 + 2*msbpos(rankpop1)) = c0drb + c1drb;
		MCHBAR16(0x606 + 2*msbpos(rankpop1)) = c0drb + c1drb;
	}

	/* Populated channel sizes in MiB */
	size0 = s->channel_capacity[0];
	size1 = s->channel_capacity[1];

	MCHBAR8(0x111) = MCHBAR8(0x111) & ~0x2;
	MCHBAR8(0x111) = MCHBAR8(0x111) | (1 << 4);

	/* Set ME UMA size in MiB */
	MCHBAR16(0x100) = ME_UMA_SIZEMB;

	/* Set ME UMA Present bit */
	MCHBAR32(0x111) = MCHBAR32(0x111) | 1;

	size = MIN(size0 - ME_UMA_SIZEMB, size1) * 2;

	MCHBAR16(0x104) = size;
	MCHBAR16(0x102) = size0 + size1 - size;

	map = 0;
	if (size0 == 0)
		map = 0;
	else if (size1 == 0)
		map |= 0x20;
	else
		map |= 0x40;

	if (size == 0)
		map |= 0x18;

	if (size0 - ME_UMA_SIZEMB >= size1)
		map |= 0x4;
	MCHBAR8(0x110) = map;
	MCHBAR16(0x10e) = 0;

	if (size1 != 0)
		offset = 0;
	else if ((size0 > size1) && ((map & 0x7) == 0x4))
		offset = size/2 + (size0 + size1 - size);
	else
		offset = size/2 + ME_UMA_SIZEMB;
	MCHBAR16(0x108) = offset;
	MCHBAR16(0x10a) = size/2;
}

static void mmap_ddr2(struct sysinfo *s)
{
	bool reclaim;
	u32 gfxsize, gttsize, tsegsize, mmiosize, tom, tolud, touud;
	u32 gfxbase, gttbase, tsegbase, reclaimbase, reclaimlimit;
	u16 ggc;
	u16 ggc2uma[] = { 0, 1, 4, 8, 16, 32, 48, 64, 128, 256, 96,
			  160, 224, 352 };
	u8 ggc2gtt[] = { 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4};

	ggc = pci_read_config16(PCI_DEV(0, 0, 0), 0x52);
	gfxsize = ggc2uma[(ggc & 0xf0) >> 4];
	gttsize = ggc2gtt[(ggc & 0xf00) >> 8];
	tsegsize = 1; // 1MB TSEG
	mmiosize = 0x800; // 2GB MMIO
	tom = s->channel_capacity[0] + s->channel_capacity[1] - ME_UMA_SIZEMB;
	tolud = MIN(0x1000 - mmiosize, tom);

	reclaim = false;
	if ((tom - tolud) > 0x40)
		reclaim = true;

	if (reclaim) {
		tolud = tolud & ~0x3f;
		tom = tom & ~0x3f;
		reclaimbase = MAX(0x1000, tom);
		reclaimlimit = reclaimbase + (MIN(0x1000, tom) - tolud) - 0x40;
	}

	touud = tom;
	if (reclaim)
		touud = reclaimlimit + 0x40;

	gfxbase = tolud - gfxsize;
	gttbase = gfxbase - gttsize;
	tsegbase = gttbase - tsegsize;

	pci_write_config16(PCI_DEV(0, 0, 0), 0xb0, tolud << 4);
	pci_write_config16(PCI_DEV(0, 0, 0), 0xa0, tom >> 6);
	if (reclaim) {
		pci_write_config16(PCI_DEV(0, 0, 0), 0x98,
					(u16)(reclaimbase >> 6));
		pci_write_config16(PCI_DEV(0, 0, 0), 0x9a,
					(u16)(reclaimlimit >> 6));
	}
	pci_write_config16(PCI_DEV(0, 0, 0), 0xa2, touud);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xa4, gfxbase << 20);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xa8, gttbase << 20);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xac, tsegbase << 20);
}

static void enhanced_ddr2(struct sysinfo *s)
{
	u8 ch, reg8;

	MCHBAR32(0xfb0) = 0x1000d024;
	MCHBAR32(0xfb4) = 0xc842;
	MCHBAR32(0xfbc) = 0xf;
	MCHBAR32(0xfc4) = 0xfe22244;
	MCHBAR8(0x12f) = 0x5c;
	MCHBAR8(0xfb0) = (MCHBAR8(0xfb0) & ~1) | 1;
	MCHBAR8(0x12f) = MCHBAR8(0x12f) | 0x2;
	MCHBAR8(0x6c0) = (MCHBAR8(0x6c0) & ~0xf0) | 0xa0;
	MCHBAR32(0xfa8) = 0x30d400;

	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR8(0x400*ch + 0x26c) = MCHBAR8(0x400*ch + 0x26c) | 1;
		MCHBAR32(0x400*ch + 0x278) = 0x88141881;
		MCHBAR16(0x400*ch + 0x27c) = 0x0041;
		MCHBAR8(0x400*ch + 0x292) = 0xf2;
		MCHBAR16(0x400*ch + 0x272) = MCHBAR16(0x400*ch + 0x272) | 0x100;
		MCHBAR8(0x400*ch + 0x243) = (MCHBAR8(0x400*ch + 0x243) & ~0x2) | 1;
		MCHBAR32(0x400*ch + 0x288) = 0x8040200;
		MCHBAR32(0x400*ch + 0x28c) = 0xff402010;
		MCHBAR32(0x400*ch + 0x290) = 0x4f2091c;
	}

	reg8 = pci_read_config8(PCI_DEV(0, 0, 0), 0xf0);
	pci_write_config8(PCI_DEV(0, 0, 0), 0xf0, reg8 | 1);
	MCHBAR32(0xfa0) = (MCHBAR32(0xfa0) & ~0x20002) | 0x2;
	MCHBAR32(0xfa4) = (MCHBAR32(0xfa4) & ~0x219100c3) | 0x219100c2;
	MCHBAR32(0x2c) = 0x44a53;
	MCHBAR32(0x30) = 0x1f5a86;
	MCHBAR32(0x34) = 0x1902810;
	MCHBAR32(0x38) = 0xf7000000;
	MCHBAR32(0x3c) = 0x23014410;
	MCHBAR32(0x40) = (MCHBAR32(0x40) & ~0x8f038000) | 0x8f038000;
	MCHBAR32(0x20) = 0x33001;
	pci_write_config8(PCI_DEV(0, 0, 0), 0xf0, reg8 & ~1);
}

static void power_ddr2(struct sysinfo *s)
{
	u32 reg1, reg2, reg3, reg4, clkgate, x592;
	u8 lane, ch;
	u8 twl = 0;
	u16 x264, x23c;

	twl = s->selected_timings.CAS - 1;
	x264 = 0x78;
	switch (s->selected_timings.mem_clk) {
	default:
	case MEM_CLOCK_667MHz:
		reg1 = 0x99;
		reg2 = 0x1048a9;
		clkgate = 0x230000;
		x23c = 0x7a89;
		break;
	case MEM_CLOCK_800MHz:
		if (s->selected_timings.CAS == 5) {
			reg1 = 0x19a;
			reg2 = 0x1048aa;
		} else {
			reg1 = 0x9a;
			reg2 = 0x2158aa;
			x264 = 0x89;
		}
		clkgate = 0x280000;
		x23c = 0x7b89;
		break;
	}
	reg3 = 0x232;
	reg4 = 0x2864;

	if (CHANNEL_IS_POPULATED(s->dimms, 0) && CHANNEL_IS_POPULATED(s->dimms, 1))
		MCHBAR32(0x14) = 0x0010461f;
	else
		MCHBAR32(0x14) = 0x0010691f;
	MCHBAR32(0x18) = 0xdf6437f7;
	MCHBAR32(0x1c) = 0x0;
	MCHBAR32(0x24) = (MCHBAR32(0x24) & ~0xe0000000) | 0x30000000;
	MCHBAR32(0x44) = (MCHBAR32(0x44) & ~0x1fef0000) | 0x6b0000;
	MCHBAR16(0x115) = (u16) reg1;
	MCHBAR32(0x117) = (MCHBAR32(0x117) & ~0xffffff) | reg2;
	MCHBAR8(0x124) = 0x7;
	MCHBAR16(0x12a) = (MCHBAR16(0x12a) & 0) | 0x80;
	MCHBAR8(0x12c) = (MCHBAR8(0x12c) & 0) | 0xa0;
	MCHBAR16(0x174) = MCHBAR16(0x174) & ~(1 << 15);
	MCHBAR16(0x188) = (MCHBAR16(0x188) & ~0x1f00) | 0x1f00;
	MCHBAR8(0x18c) = MCHBAR8(0x18c) & ~0x8;
	MCHBAR8(0x192) = (MCHBAR8(0x192) & ~1) | 1;
	MCHBAR8(0x193) = (MCHBAR8(0x193) & ~0xf) | 0xf;
	MCHBAR16(0x1b4) = (MCHBAR16(0x1b4) & ~0x480) | 0x80;
	MCHBAR16(0x210) = (MCHBAR16(0x210) & ~0x1fff) | 0x3f; // | clockgatingiii
	MCHBAR32(0x6d1) = (MCHBAR32(0x6d1) & ~0xff03ff) | 0x100 | clkgate;
	MCHBAR8(0x212) = (MCHBAR8(0x212) & ~0x7f) | 0x7f;
	MCHBAR32(0x2c0) = (MCHBAR32(0x2c0) & ~0xffff0) | 0xcc5f0;
	MCHBAR8(0x2c4) = (MCHBAR8(0x2c4) & ~0x70) | 0x70;
	MCHBAR32(0x2d1) = (MCHBAR32(0x2d1) & ~0xffffff) | 0xff2831; // | clockgatingi
	MCHBAR32(0x2d4) = 0x40453600;
	MCHBAR32(0x300) = 0xc0b0a08;
	MCHBAR32(0x304) = 0x6040201;
	MCHBAR32(0x30c) = (MCHBAR32(0x30c) & ~0x43c0f) | 0x41405;
	MCHBAR16(0x610) = 0x232;
	MCHBAR16(0x612) = 0x2864;
	MCHBAR32(0x62c) = (MCHBAR32(0x62c) & ~0xc000000) | 0x4000000;
	MCHBAR32(0xae4) = 0;
	MCHBAR32(0xc00) = (MCHBAR32(0xc00) & ~0xf0000) | 0x10000;
	MCHBAR32(0xf00) = 0x393a3b3c;
	MCHBAR32(0xf04) = 0x3d3e3f40;
	MCHBAR32(0xf08) = 0x393a3b3c;
	MCHBAR32(0xf0c) = 0x3d3e3f40;
	MCHBAR32(0xf18) = MCHBAR32(0xf18) & ~0xfff00001;
	MCHBAR32(0xf48) = 0xfff0ffe0;
	MCHBAR32(0xf4c) = 0xffc0ff00;
	MCHBAR32(0xf50) = 0xfc00f000;
	MCHBAR32(0xf54) = 0xc0008000;
	MCHBAR32(0xf6c) = (MCHBAR32(0xf6c) & ~0xffff0000) | 0xffff0000;
	MCHBAR32(0xfac) = MCHBAR32(0xfac) & ~0x80000000;
	MCHBAR32(0xfb8) = MCHBAR32(0xfb8) & ~0xff000000;
	MCHBAR32(0xfbc) = (MCHBAR32(0xfbc) & ~0x7f800) | 0xf000;
	MCHBAR32(0x1104) = 0x3003232;
	MCHBAR32(0x1108) = 0x74;
	if (s->selected_timings.fsb_clk == FSB_CLOCK_800MHz)
		MCHBAR32(0x110c) = 0xaa;
	else
		MCHBAR32(0x110c) = 0x100;
	MCHBAR32(0x1110) = 0x10810350 & ~0x78;
	MCHBAR32(0x1114) = 0;
	if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz)
		twl = 5;
	else
		twl = 6;
	x592 = 0xff;
	if (pci_read_config8(PCI_DEV(0, 0, 0), 0x8) < 3)
		x592 = ~0x4;

	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR8(0x400*ch + 0x239) = twl + 15;
		MCHBAR16(0x400*ch + 0x23c) = x23c;
		MCHBAR32(0x400*ch + 0x248) = (MCHBAR32(0x400*ch + 0x248) & ~0x706033) | 0x406033;
		MCHBAR32(0x400*ch + 0x260) = (MCHBAR32(0x400*ch + 0x260) & ~(1 << 16)) | (1 << 16);
		MCHBAR8(0x400*ch + 0x264) = x264;
		MCHBAR8(0x400*ch + 0x592) = (MCHBAR8(0x400*ch + 0x592) & ~0x3f) | (0x3c & x592);
		MCHBAR8(0x400*ch + 0x593) = (MCHBAR8(0x400*ch + 0x593) & ~0x1f) | 0x1e;
	}

	for (lane = 0; lane < 8; lane++)
		MCHBAR8(0x561 + (lane << 2)) = MCHBAR8(0x561 + (lane << 2)) & ~(1 << 3);
}

void raminit_ddr2(struct sysinfo *s)
{
	u8 ch;
	u8 r, bank;
	u32 reg32;

	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		// Clear self refresh
		MCHBAR32(PMSTS_MCHBAR) = MCHBAR32(PMSTS_MCHBAR)
			| PMSTS_BOTH_SELFREFRESH;

		// Clear host clk gate reg
		MCHBAR32(0x1c) = MCHBAR32(0x1c) | 0xffffffff;

		// Select DDR2
		MCHBAR8(0x1a8) = MCHBAR8(0x1a8) & ~0x4;

		// Set freq
		MCHBAR32(0xc00) = (MCHBAR32(0xc00) & ~0x70) |
			(s->selected_timings.mem_clk << 4) | (1 << 10);

		// Overwrite freq if chipset rejects it
		s->selected_timings.mem_clk = (MCHBAR8(0xc00) & 0x70) >> 4;
		if (s->selected_timings.mem_clk > (s->max_fsb + 3))
			die("Error: DDR is faster than FSB, halt\n");
	}

	// Program clock crossing
	clkcross_ddr2(s);
	printk(BIOS_DEBUG, "Done clk crossing\n");

	// DDR2 IO
	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		setioclk_ddr2(s);
		printk(BIOS_DEBUG, "Done I/O clk\n");
	}

	// Grant to launch
	launch_ddr2(s);
	printk(BIOS_DEBUG, "Done launch\n");

	// Program DDR2 timings
	timings_ddr2(s);
	printk(BIOS_DEBUG, "Done timings\n");

	// Program DLL
	dll_ddr2(s);

	// RCOMP
	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		rcomp_ddr2(s);
		printk(BIOS_DEBUG, "RCOMP\n");
	}

	// ODT
	odt_ddr2(s);
	printk(BIOS_DEBUG, "Done ODT\n");

	// RCOMP update
	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		while ((MCHBAR8(0x130) & 1) != 0)
			;
		printk(BIOS_DEBUG, "Done RCOMP update\n");
	}

	// Set defaults
	MCHBAR32(0x260) = (MCHBAR32(0x260) & ~1) | 0xf00000;
	MCHBAR32(0x660) = (MCHBAR32(0x660) & ~1) | 0xf00000;
	MCHBAR32(0x208) = 0x01010101;
	MCHBAR32(0x608) = 0x01010101;
	MCHBAR32(0x200) = 0x00040002;
	MCHBAR32(0x204) = 0x00080006;
	MCHBAR32(0x600) = 0x00040002;
	MCHBAR32(0x604) = 0x00100006;
	MCHBAR8(0x111) = MCHBAR8(0x111) | 0x2;
	MCHBAR32(0x104) = 0;
	MCHBAR16(0x102) = 0x400;
	MCHBAR8(0x100) = (2 << 5) | (3 << 3);
	MCHBAR16(0x10e) = 0;
	MCHBAR32(0x108) = 0;
	pci_write_config16(PCI_DEV(0, 0, 0), 0xb0, 0x4000);
	pci_write_config16(PCI_DEV(0, 0, 0), 0xa0, 0x0010);
	pci_write_config16(PCI_DEV(0, 0, 0), 0xa2, 0x0400);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xa4, 0x40000000);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xa8, 0x40000000);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xac, 0x40000000);

	// IOBUFACT
	if (CHANNEL_IS_POPULATED(s->dimms, 0)) {
		MCHBAR8(0x5dd) = (MCHBAR8(0x5dd) & ~0x3f) | 0x3f;
		MCHBAR8(0x5d8) = MCHBAR8(0x5d8) | 0x7;
	}
	if (CHANNEL_IS_POPULATED(s->dimms, 1)) {
		if (pci_read_config8(PCI_DEV(0, 0, 0), 0x8) < 2) {
			MCHBAR8(0x5dd) = (MCHBAR8(0x5dd) & ~0x3f) | 0x3f;
			MCHBAR8(0x5d8) = MCHBAR8(0x5d8) | 1;
		}
		MCHBAR8(0x9dd) = (MCHBAR8(0x9dd) & ~0x3f) | 0x3f;
		MCHBAR8(0x9d8) = MCHBAR8(0x9d8) | 0x7;
	}

	// Pre jedec
	MCHBAR8(0x40) = MCHBAR8(0x40) | 0x2;
	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR32(0x400*ch + 0x260) = MCHBAR32(0x400*ch + 0x260) | (1 << 27);
	}
	MCHBAR16(0x212) = (MCHBAR16(0x212) & ~0xf000) | 0xf000;
	MCHBAR16(0x212) = (MCHBAR16(0x212) & ~0xf00) | 0xf00;
	printk(BIOS_DEBUG, "Done pre-jedec\n");

	// JEDEC reset
	if (s->boot_path != BOOT_PATH_RESUME)
		jedec_ddr2(s);

	printk(BIOS_DEBUG, "Done jedec steps\n");

	// After JEDEC reset
	MCHBAR8(0x40) = MCHBAR8(0x40) & ~0x2;
	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz)
			reg32 = (2 << 18) | (3 << 13) | (5 << 8);
		else
			reg32 = (2 << 18) | (3 << 13) | (4 << 8);
		MCHBAR32(0x400*ch + 0x274) = (MCHBAR32(0x400*ch + 0x274) & ~0xfff00) | reg32;
		MCHBAR8(0x400*ch + 0x274) = MCHBAR8(0x400*ch + 0x274) & ~0x80;
		MCHBAR8(0x400*ch + 0x26c) = MCHBAR8(0x400*ch + 0x26c) | 1;
		MCHBAR32(0x400*ch + 0x278) = 0x88141881;
		MCHBAR16(0x400*ch + 0x27c) = 0x41;
		MCHBAR8(0x400*ch + 0x292) = 0xf2;
		MCHBAR8(0x400*ch + 0x271) = (MCHBAR8(0x400*ch + 0x271) & ~0xe) | 0xe;
	}
	MCHBAR8(0x2c4) = MCHBAR8(0x2c4) | 0x8;
	MCHBAR8(0x2c3) = MCHBAR8(0x2c3) | 0x40;
	MCHBAR8(0x2c4) = MCHBAR8(0x2c4) | 0x4;

	printk(BIOS_DEBUG, "Done post-jedec\n");

	// Set DDR2 init complete
	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR32(0x400*ch + 0x268) = (MCHBAR32(0x400*ch + 0x268) & ~0xc0000000) | 0xc0000000;
	}

	// Receive enable
	sdram_program_receive_enable(s);
	printk(BIOS_DEBUG, "Done rcven\n");

	// Finish rcven
	FOR_EACH_CHANNEL(ch) {
		MCHBAR8(0x400*ch + 0x5d8) = MCHBAR8(0x400*ch + 0x5d8) & ~0xe;
		MCHBAR8(0x400*ch + 0x5d8) = MCHBAR8(0x400*ch + 0x5d8) | 0x2;
		MCHBAR8(0x400*ch + 0x5d8) = MCHBAR8(0x400*ch + 0x5d8) | 0x4;
		MCHBAR8(0x400*ch + 0x5d8) = MCHBAR8(0x400*ch + 0x5d8) | 0x8;
	}
	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) | 0x80;
	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) & ~0x80;
	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) | 0x80;

	// Dummy writes / reads
	if (s->boot_path == BOOT_PATH_NORMAL) {
		volatile u32 data;
		FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
			for (bank = 0; bank < 4; bank++) {
				reg32 = (ch << 29) | (r*0x8000000) |
					(bank << 12);
				write32((u32 *)reg32, 0xffffffff);
				data = read32((u32 *)reg32);
				printk(BIOS_DEBUG, "Wrote ones,");
				printk(BIOS_DEBUG, "  Read: [0x%08x]=0x%08x\n",
					reg32, data);
				write32((u32 *)reg32, 0x00000000);
				data = read32((u32 *)reg32);
				printk(BIOS_DEBUG, "Wrote zeros,");
				printk(BIOS_DEBUG, " Read: [0x%08x]=0x%08x\n",
					reg32, data);
			}
		}
	}
	printk(BIOS_DEBUG, "Done dummy reads\n");

	// XXX tRD

	// XXX Write training

	// XXX Read training

	// DRADRB
	dradrb_ddr2(s);
	printk(BIOS_DEBUG, "Done DRADRB\n");

	// Memory map
	mmap_ddr2(s);
	printk(BIOS_DEBUG, "Done memory map\n");

	// Enhanced mode
	enhanced_ddr2(s);
	printk(BIOS_DEBUG, "Done enhanced mode\n");

	// Periodic RCOMP
	MCHBAR16(0x160) = (MCHBAR16(0x160) & ~0xfff) | 0x999;
	MCHBAR16(0x1b4) = MCHBAR16(0x1b4) | 0x3000;
	MCHBAR8(0x130) = MCHBAR8(0x130) | 0x82;
	printk(BIOS_DEBUG, "Done PRCOMP\n");

	// Power settings
	power_ddr2(s);
	printk(BIOS_DEBUG, "Done power settings\n");

	// ME related
	/*
	 * FIXME: This locks some registers like bit1 of GGC
	 * and is only needed in case of ME being used.
	 */
	if (ME_UMA_SIZEMB != 0) {
		if (RANK_IS_POPULATED(s->dimms, 0, 0)
				|| RANK_IS_POPULATED(s->dimms, 1, 0))
			MCHBAR8(0xa2f) = MCHBAR8(0xa2f) | (1 << 0);
		if (RANK_IS_POPULATED(s->dimms, 0, 1)
				|| RANK_IS_POPULATED(s->dimms, 1, 1))
			MCHBAR8(0xa2f) = MCHBAR8(0xa2f) | (1 << 1);
		MCHBAR32(0xa30) = MCHBAR32(0xa30) | (1 << 26);
	}

	printk(BIOS_DEBUG, "Done ddr2\n");
}
