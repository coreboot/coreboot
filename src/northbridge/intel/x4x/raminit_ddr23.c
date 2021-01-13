/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <stdint.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <delay.h>
#if CONFIG(SOUTHBRIDGE_INTEL_I82801GX)
#include <southbridge/intel/i82801gx/i82801gx.h>
#else
#include <southbridge/intel/i82801jx/i82801jx.h>
#endif
#include <string.h>
#include "raminit.h"
#include "x4x.h"

#define ME_UMA_SIZEMB 0

u32 fsb_to_mhz(u32 speed)
{
	return (speed * 267) + 800;
}

u32 ddr_to_mhz(u32 speed)
{
	static const u16 mhz[] = { 0, 0, 667, 800, 1067, 1333 };

	if (speed <= 1 || speed >= ARRAY_SIZE(mhz))
		die("RAM init: invalid memory speed %u\n", speed);

	return mhz[speed];
}

static void program_crossclock(struct sysinfo *s)
{
	u8 i, j;
	u32 reg32;
	MCHBAR16_OR(0xc1c, (1 << 15));

	static const u32 clkxtab[6][3][13] = {
		/* MEMCLK 400 N/A */
		{{}, {}, {} },
		/* MEMCLK 533 N/A */
		{{}, {}, {} },
		/* MEMCLK 667
		 * FSB 800 */
		{{0x1f1f1f1f, 0x0d07070b, 0x00000000, 0x10000000,
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
		 0x00000000, 0x00000000, 0x00020100, 0x00000000,
		 0x04080100},
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
	reg32 = clkxtab[i][j][1];
	if (s->spd_type == DDR3 && s->max_fsb == FSB_CLOCK_1333MHz
		&& s->selected_timings.mem_clk == MEM_CLOCK_800MHz) {
		reg32 &= ~(0xff << 24);
		reg32 |= 0x3d << 24;
	}
	MCHBAR32(0xc50) = reg32;
	MCHBAR32(0xc54) = clkxtab[i][j][2];
	MCHBAR8_OR(0xc08, (1 << 7));
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

static void setioclk_dram(struct sysinfo *s)
{
	MCHBAR32(0x1bc) = 0x08060402;
	MCHBAR16_OR(0x1c0, 0x200);
	MCHBAR16_OR(0x1c0, 0x100);
	MCHBAR16_OR(0x1c0, 0x20);
	MCHBAR16_AND(0x1c0, ~1);
	switch (s->selected_timings.mem_clk) {
	default:
	case MEM_CLOCK_800MHz:
	case MEM_CLOCK_1066MHz:
		MCHBAR8_AND_OR(0x5d9, ~0x2, 0x2);
		MCHBAR8_AND_OR(0x9d9, ~0x2, 0x2);
		MCHBAR8_AND_OR(0x189, ~0xf0, 0xc0);
		MCHBAR8_AND_OR(0x189, ~0xf0, 0xe0);
		MCHBAR8_AND_OR(0x189, ~0xf0, 0xa0);
		break;
	case MEM_CLOCK_667MHz:
	case MEM_CLOCK_1333MHz:
		MCHBAR8_AND(0x5d9, ~0x2);
		MCHBAR8_AND(0x9d9, ~0x2);
		MCHBAR8_AND_OR(0x189, ~0xf0, 0x40);
		break;
	}
	MCHBAR32_OR(0x594, 1 << 31);
	MCHBAR32_OR(0x994, 1 << 31);
}

static void launch_dram(struct sysinfo *s)
{
	u8 i;
	u32 launch1;
	u32 launch2 = 0;

	static const u32 ddr3_launch1_tab[2][3] = {
		/* 1N */
		{0x58000007, /* DDR3 800 */
		 0x58000007, /* DDR3 1067 */
		 0x58100107}, /* DDR3 1333 */
		/* 2N */
		{0x58001117, /* DDR3 800 */
		 0x58001117, /* DDR3 1067 */
		 0x58001117} /* DDR3 1333 */
	};

	static const u32 ddr3_launch2_tab[2][3][6] = {
		{ /* 1N */
			/* DDR3 800 */
			{0x08030000,	/* CL = 5 */
			 0x0C040100},	/* CL = 6 */
			/* DDR3 1066 */
			{0x00000000,	/* CL = 5 */
			 0x00000000,	/* CL = 6 */
			 0x10050100,	/* CL = 7 */
			 0x14260200},	/* CL = 8 */
			/* DDR3 1333 */
			{0x00000000,	/* CL = 5 */
			 0x00000000,	/* CL = 6 */
			 0x00000000,	/* CL = 7 */
			 0x14060000,	/* CL = 8 */
			 0x18070100,	/* CL = 9 */
			 0x1C280200},	/* CL = 10 */

		},
		{ /* 2N */
			/* DDR3 800 */
			{0x00040101,	/* CL = 5 */
			 0x00250201},	/* CL = 6 */
			/* DDR3 1066 */
			{0x00000000,	/* CL = 5 */
			 0x00050101,	/* CL = 6 */
			 0x04260201,	/* CL = 7 */
			 0x08470301},	/* CL = 8 */
			/* DDR3 1333 */
			{0x00000000,	/* CL = 5 */
			 0x00000000,	/* CL = 6 */
			 0x00000000,	/* CL = 7 */
			 0x08070100,	/* CL = 8 */
			 0x0C280200,	/* CL = 9 */
			 0x10490300}	/* CL = 10 */
		}
	};

	if (s->spd_type == DDR2) {
		launch1 = 0x58001117;
		if (s->selected_timings.CAS == 5)
			launch2 = 0x00220201;
		else if (s->selected_timings.CAS == 6)
			launch2 = 0x00230302;
		else
			die("Unsupported CAS\n");
	} else { /* DDR3 */
		/* Default 2N mode */
		s->nmode = 2;

		if (s->selected_timings.mem_clk <= MEM_CLOCK_1066MHz)
			s->nmode = 1;
		/* 2N on DDR3 1066 with 2 dimms per channel */
		if ((s->selected_timings.mem_clk == MEM_CLOCK_1066MHz) &&
			(BOTH_DIMMS_ARE_POPULATED(s->dimms, 0) ||
				BOTH_DIMMS_ARE_POPULATED(s->dimms, 1)))
			s->nmode = 2;
		launch1 = ddr3_launch1_tab[s->nmode - 1]
			[s->selected_timings.mem_clk - MEM_CLOCK_800MHz];
		launch2 = ddr3_launch2_tab[s->nmode - 1]
			[s->selected_timings.mem_clk - MEM_CLOCK_800MHz]
			[s->selected_timings.CAS - 5];
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		MCHBAR32(0x400*i + 0x220) = launch1;
		MCHBAR32(0x400*i + 0x224) = launch2;
		MCHBAR32(0x400*i + 0x21c) = 0;
		MCHBAR32_OR(0x400*i + 0x248, 1 << 23);
	}

	MCHBAR32_AND_OR(0x2c0, ~0x58000000, 0x48000000);
	MCHBAR32_OR(0x2c0, 0x1e0);
	MCHBAR32_AND_OR(0x2c4, ~0xf, 0xc);
	if (s->spd_type == DDR3)
		MCHBAR32_OR(0x2c4, 0x100);
}

static void write_txdll_tap_pi(u8 ch, u16 reg, u8 tap, u8 pi)
{
	MCHBAR8_AND_OR(0x400 * ch + reg, ~0x7f, pi << 4 | tap);
}

static void clkset0(u8 ch, const struct dll_setting *setting)
{
	MCHBAR16_AND_OR(0x400*ch + 0x5a0, ~0xc440,
		(setting->clk_delay << 14) |
		(setting->db_sel << 6) |
		(setting->db_en << 10));
	write_txdll_tap_pi(ch, 0x581, setting->tap, setting->pi);
}

static void clkset1(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32_AND_OR(0x400*ch + 0x5a0, ~0x30880,
		(setting->clk_delay << 16) |
		(setting->db_sel << 7) |
		(setting->db_en << 11));
	write_txdll_tap_pi(ch, 0x582, setting->tap, setting->pi);
}

static void ctrlset0(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32_AND_OR(0x400*ch + 0x59c, ~0x3300000,
		(setting->clk_delay << 24) |
		(setting->db_sel << 20) |
		(setting->db_en << 21));
	write_txdll_tap_pi(ch, 0x584, setting->tap, setting->pi);
}

static void ctrlset1(u8 ch, const struct dll_setting *setting)
{
	MCHBAR32_AND_OR(0x400*ch + 0x59c, ~0x18c00000,
		(setting->clk_delay << 27) |
		(setting->db_sel << 22) |
		(setting->db_en << 23));
	write_txdll_tap_pi(ch, 0x585, setting->tap, setting->pi);
}

static void ctrlset2(u8 ch, const struct dll_setting *setting)
{
	/*
	 * MRC uses an incorrect mask when programming this register, but
	 * the reset default value is zero and it is only programmed once.
	 * As it makes no difference, we can safely use the correct mask.
	 */
	MCHBAR32_AND_OR(0x400*ch + 0x598, ~0xf000,
		(setting->clk_delay << 14) |
		(setting->db_sel << 12) |
		(setting->db_en << 13));
	write_txdll_tap_pi(ch, 0x586, setting->tap, setting->pi);
}

static void ctrlset3(u8 ch, const struct dll_setting *setting)
{
	/*
	 * MRC uses an incorrect mask when programming this register, but
	 * the reset default value is zero and it is only programmed once.
	 * As it makes no difference, we can safely use the correct mask.
	 */
	MCHBAR32_AND_OR(0x400*ch + 0x598, ~0xf00,
		(setting->clk_delay << 10) |
		(setting->db_sel << 8) |
		(setting->db_en << 9));
	write_txdll_tap_pi(ch, 0x587, setting->tap, setting->pi);
}

static void cmdset(u8 ch, const struct dll_setting *setting)
{
	MCHBAR8_AND_OR(0x400*ch + 0x598, ~0x30, setting->clk_delay << 4);
	MCHBAR8_AND_OR(0x400*ch + 0x594, ~0x60,
		(setting->db_sel << 5) |
		(setting->db_en << 6));
	write_txdll_tap_pi(ch, 0x580, setting->tap, setting->pi);
}

/**
 * All finer DQ and DQS DLL settings are set to the same value
 * for each rank in a channel, while coarse is common.
 */
void dqsset(u8 ch, u8 lane, const struct dll_setting *setting)
{
	int rank;

	MCHBAR32_AND_OR(0x400 * ch + 0x5fc, ~(1 << (lane * 4 + 1)),
		setting->coarse << (lane * 4 + 1));

	for (rank = 0; rank < 4; rank++) {
		MCHBAR32_AND_OR(0x400 * ch + 0x5b4 + rank * 4, ~(0x201 << lane),
			(setting->db_en << (9 + lane)) |
			(setting->db_sel << lane));

		MCHBAR32_AND_OR(0x400*ch + 0x5c8 + rank * 4,
			~(0x3 << (16 + lane * 2)),
			setting->clk_delay << (16+lane * 2));

		write_txdll_tap_pi(ch, 0x520 + lane * 4 + rank, setting->tap, setting->pi);
	}
}

void dqset(u8 ch, u8 lane, const struct dll_setting *setting)
{
	int rank;
	MCHBAR32_AND_OR(0x400 * ch + 0x5fc, ~(1 << (lane * 4)),
		setting->coarse << (lane * 4));

	for (rank = 0; rank < 4; rank++) {
		MCHBAR32_AND_OR(0x400 * ch + 0x5a4 + rank * 4, ~(0x201 << lane),
			(setting->db_en << (9 + lane)) |
			(setting->db_sel << lane));

		MCHBAR32_AND_OR(0x400 * ch + 0x5c8 + rank * 4,
			~(0x3 << (lane * 2)), setting->clk_delay << (2 * lane));

		write_txdll_tap_pi(ch, 0x500 + lane * 4 + rank, setting->tap, setting->pi);
	}
}

void rt_set_dqs(u8 channel, u8 lane, u8 rank,
		struct rt_dqs_setting *dqs_setting)
{
	u16 saved_tap = MCHBAR16(0x540 + 0x400 * channel + lane * 4);
	u16 saved_pi = MCHBAR16(0x542 + 0x400 * channel + lane * 4);
	printk(RAM_SPEW, "RT DQS: ch%d, r%d, L%d: %d.%d\n", channel, rank, lane,
		dqs_setting->tap,
		dqs_setting->pi);

	saved_tap &= ~(0xf << (rank * 4));
	saved_tap |= dqs_setting->tap << (rank * 4);
	MCHBAR16(0x540 + 0x400 * channel + lane * 4) = saved_tap;

	saved_pi &= ~(0x7 << (rank * 3));
	saved_pi |= dqs_setting->pi << (rank * 3);
	MCHBAR16(0x542 + 0x400 * channel + lane * 4) = saved_pi;
}

static void program_timings(struct sysinfo *s)
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

	u16 fsb_to_ps[3] = {
		5000, /*  800 */
		3750, /* 1067 */
		3000  /* 1333 */
	};

	u16 ddr_to_ps[6] = {
		5000, /*  400 */
		3750, /*  533 */
		3000, /*  667 */
		2500, /*  800 */
		1875, /* 1067 */
		1500  /* 1333 */
	};

	u16 lut1[6] = {
		0,
		0,
		2600,
		3120,
		4171,
		5200
	};

	static const u8 ddr3_turnaround_tab[3][6][4] = {
		{ /* DDR3 800 */
			{0x9, 0x7, 0x7, 0x9},	/* CL = 5 */
			{0x9, 0x7, 0x8, 0x8},	/* CL = 6 */
		},
		{ /* DDR3 1066 */
			{0x0, 0x0, 0x0, 0x0},	/* CL = 5 - Not supported */
			{0x9, 0x7, 0x7, 0x9},	/* CL = 6 */
			{0x9, 0x7, 0x8, 0x8},	/* CL = 7 */
			{0x9, 0x7, 0x9, 0x7}	/* CL = 8 */
		},
		{ /* DDR3 1333 */
			{0x0, 0x0, 0x0, 0x0},	/* CL = 5 - Not supported */
			{0x0, 0x0, 0x0, 0x0},	/* CL = 6 - Not supported */
			{0x0, 0x0, 0x0, 0x0},	/* CL = 7 - Not supported */
			{0x9, 0x7, 0x9, 0x8},	/* CL = 8 */
			{0x9, 0x7, 0xA, 0x7},	/* CL = 9 */
			{0x9, 0x7, 0xB, 0x6},	/* CL = 10 */
		}
	};

	/* [DDR freq][0x26F & 1][pagemod] */
	static const u8 ddr2_x252_tab[2][2][2] =  {
		{ /* DDR2 667 */
			{12, 16},
			{14, 18}
		},
		{ /* DDR2 800 */
			{14, 18},
			{16, 20}
		}
	};

	static const u8 ddr3_x252_tab[3][2][2] =  {
		{ /* DDR3 800 */
			{16, 20},
			{18, 22}
		},
		{ /* DDR3 1067 */
			{20, 26},
			{26, 26}
		},
		{ /* DDR3 1333 */
			{20, 30},
			{22, 32},
		}
	};

	if (s->spd_type == DDR2) {
		ta1 = 6;
		ta2 = 6;
		ta3 = 5;
		ta4 = 8;
	} else {
		int ddr3_idx = s->selected_timings.mem_clk - MEM_CLOCK_800MHz;
		int cas_idx = s->selected_timings.CAS - 5;
		ta1 = ddr3_turnaround_tab[ddr3_idx][cas_idx][0];
		ta2 = ddr3_turnaround_tab[ddr3_idx][cas_idx][1];
		ta3 = ddr3_turnaround_tab[ddr3_idx][cas_idx][2];
		ta4 = ddr3_turnaround_tab[ddr3_idx][cas_idx][3];
	}

	if (s->spd_type == DDR2)
		twl = s->selected_timings.CAS - 1;
	else /* DDR3 */
		twl = s->selected_timings.mem_clk - MEM_CLOCK_800MHz + 5;

	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		if (s->dimms[i].n_banks == N_BANKS_8) {
			trpmod = 1;
			bankmod = 0;
		}
		if (s->dimms[i].page_size == 2048)
			pagemod = 1;
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		MCHBAR8_OR(0x400*i + 0x26f, 0x3);
		MCHBAR8_AND_OR(0x400*i + 0x228, ~0x7, 0x2);
		/* tWL - x ?? */
		MCHBAR8_AND_OR(0x400*i + 0x240, ~0xf0, 0 << 4);
		MCHBAR8_AND_OR(0x400*i + 0x240, ~0xf, adjusted_cas);
		MCHBAR16_AND_OR(0x400*i + 0x265, ~0x3f00,
			(adjusted_cas + 9) << 8);

		reg16 = (s->selected_timings.tRAS << 11) |
			((twl + 4 + s->selected_timings.tWR) << 6) |
			((2 + MAX(s->selected_timings.tRTP, 2)) << 2) | 1;
		MCHBAR16(0x400*i + 0x250) = reg16;

		reg32 = (bankmod << 21) |
			(s->selected_timings.tRRD << 17) |
			(s->selected_timings.tRP << 13) |
			((s->selected_timings.tRP + trpmod) << 9) |
			s->selected_timings.tRFC;
		if (bankmod == 0) {
			reg8 = (MCHBAR8(0x400*i + 0x26f) >> 1) & 1;
			if (s->spd_type == DDR2)
				reg32 |= ddr2_x252_tab[s->selected_timings.mem_clk
						- MEM_CLOCK_667MHz][reg8][pagemod]
					<< 22;
			else
				reg32 |= ddr3_x252_tab[s->selected_timings.mem_clk
						- MEM_CLOCK_800MHz][reg8][pagemod]
					<< 22;
		}
		MCHBAR32(0x400*i + 0x252) = reg32;

		MCHBAR16(0x400*i + 0x256) = (s->selected_timings.tRCD << 12) |
			(0x4 << 8) | (ta2 << 4) | ta4;

		MCHBAR32(0x400*i + 0x258) = (s->selected_timings.tRCD << 17) |
			((twl + 4 + s->selected_timings.tWTR) << 12) |
			(ta3 << 8) | (4 << 4) | ta1;

		MCHBAR16(0x400*i + 0x25b) = ((s->selected_timings.tRP + trpmod) << 9) |
			s->selected_timings.tRFC;

		MCHBAR16_AND_OR(0x400*i + 0x260, ~0x3fe,
			(s->spd_type == DDR2 ? 100 : 256) << 1);
		MCHBAR8(0x400*i + 0x264) = 0xff;
		MCHBAR8_AND_OR(0x400*i + 0x25d, ~0x3f,
			s->selected_timings.tRAS);
		MCHBAR16(0x400*i + 0x244) = 0x2310;

		switch (s->selected_timings.mem_clk) {
		case MEM_CLOCK_667MHz:
			reg8 = 0;
			break;
		default:
			reg8 = 1;
			break;
		}

		MCHBAR8_AND_OR(0x400*i + 0x246, ~0x1f, (reg8 << 2) | 1);

		fsb = fsb_to_ps[s->selected_timings.fsb_clk];
		ddr = ddr_to_ps[s->selected_timings.mem_clk];
		reg32 = (u32)((s->selected_timings.CAS + 7 + reg8) * ddr);
		reg32 = (u32)((reg32 / fsb) << 8);
		reg32 |= 0x0e000000;
		if ((fsb_to_mhz(s->selected_timings.fsb_clk) /
		     ddr_to_mhz(s->selected_timings.mem_clk)) > 2) {
			reg32 |= 1 << 24;
		}
		MCHBAR32_AND_OR(0x400*i + 0x248, ~0x0f001f00, reg32);

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
		MCHBAR16_AND_OR(0x400*i + 0x24d, ~0x1ff, reg16);
		MCHBAR16(0x400*i + 0x25e) = 0x15a5;
		MCHBAR32_AND(0x400*i + 0x265, ~0x1f);
		MCHBAR32_AND_OR(0x400*i + 0x269, ~0x000fffff,
			(0x3f << 14) | lut1[s->selected_timings.mem_clk]);
		MCHBAR8_OR(0x400*i + 0x274, 1);
		MCHBAR8_AND(0x400*i + 0x24c, ~0x3);

		reg16 = 0;
		if (s->spd_type == DDR2) {
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
		} else { /* DDR3 */
			switch (s->selected_timings.mem_clk) {
			default:
			case MEM_CLOCK_800MHz:
			case MEM_CLOCK_1066MHz:
				reg16 = 1;
				break;
			case MEM_CLOCK_1333MHz:
				reg16 = 2;
				break;
			}
		}

		reg16 &= 0x7;
		reg16 += twl + 9;
		reg16 <<= 10;
		MCHBAR16_AND_OR(0x400*i + 0x24d, ~0x7c00, reg16);
		MCHBAR8_AND_OR(0x400*i + 0x267, ~0x3f, 0x13);
		MCHBAR8_AND_OR(0x400*i + 0x268, ~0xff, 0x4a);

		reg16 = (MCHBAR16(0x400*i + 0x269) & 0xc000) >> 2;
		reg16 += 2 << 12;
		reg16 |= (0x15 << 6) | 0x1f;
		MCHBAR16_AND_OR(0x400*i + 0x26d, ~0x7fff, reg16);

		reg32 = (1 << 25) | (6 << 27);
		MCHBAR32_AND_OR(0x400*i + 0x269, ~0xfa300000, reg32);
		MCHBAR8_AND(0x400*i + 0x271, ~0x80);
		MCHBAR8_AND(0x400*i + 0x274, ~0x6);
	} /* END EACH POPULATED CHANNEL */

	reg16 = 0x1f << 5;
	reg16 |= 0xe << 10;
	MCHBAR16_AND_OR(0x125, ~0x3fe0, reg16);
	MCHBAR16_AND_OR(0x127, ~0x7ff, 0x540);
	MCHBAR8_OR(0x129, 0x1f);
	MCHBAR8_OR(0x12c, 0xa0);
	MCHBAR32_AND_OR(0x241, ~0x1ffff, 0x11);
	MCHBAR32_AND_OR(0x641, ~0x1ffff, 0x11);
	MCHBAR8_AND(0x246, ~0x10);
	MCHBAR8_AND(0x646, ~0x10);
	MCHBAR32(0x120) = (2 << 29) | (1 << 28) | (1 << 23) | 0xd7f5f;
	reg8 = (u8)((MCHBAR32(0x252) & 0x1e000) >> 13);
	MCHBAR8_AND_OR(0x12d, ~0xf0, reg8 << 4);
	reg8 = (u8)((MCHBAR32(0x258) & 0x1e0000) >> 17);
	MCHBAR8_AND_OR(0x12d, ~0xf, reg8);
	MCHBAR8(0x12f) = 0x4c;
	reg32 = (1 << 31) | (0x80 << 14) | (1 << 13) | (0xa << 9);
	if (s->spd_type == DDR3) {
		MCHBAR8(0x114) = 0x42;
		reg16 = (512 - MAX(5, s->selected_timings.tRFC + 10000
					/ ddr_to_ps[s->selected_timings.mem_clk]))
					/ 2;
		reg16 &= 0x1ff;
		reg32 = (reg16 << 22) | (0x80 << 14) | (0xa << 9);
	}
	MCHBAR32_AND_OR(0x6c0, ~0xffffff00, reg32);
	MCHBAR8_AND_OR(0x6c4, ~0x7, 0x2);
}

static void program_dll(struct sysinfo *s)
{
	u8 i, j, r, reg8, clk, async = 0;
	u16 reg16 = 0;
	u32 reg32 = 0;

	const u8 rank2clken[8] = { 0x04, 0x01, 0x20, 0x08, 0x01, 0x04,
				   0x08, 0x10 };

	MCHBAR16_AND_OR(0x180, ~0x7e06, 0xc04);
	MCHBAR16_AND_OR(0x182, ~0x3ff, 0xc8);
	MCHBAR16_AND_OR(0x18a, ~0x1f1f, 0x0f0f);
	MCHBAR16_AND_OR(0x1b4, ~0x8020, 0x100);
	MCHBAR8_AND_OR(0x194, ~0x77, 0x33);
	switch (s->selected_timings.mem_clk) {
	default:
	case MEM_CLOCK_667MHz:
	case MEM_CLOCK_1333MHz:
		reg16 = (0xa << 9) | 0xa;
		break;
	case MEM_CLOCK_800MHz:
		reg16 = (0x9 << 9) | 0x9;
		break;
	case MEM_CLOCK_1066MHz:
		reg16 = (0x7 << 9) | 0x7;
		break;
	}
	MCHBAR16_AND_OR(0x19c, ~0x1e0f, reg16);
	MCHBAR16_AND_OR(0x19c, ~0x2030,  0x2010);
	udelay(1);
	MCHBAR16_AND(0x198, ~0x100);

	MCHBAR16_AND_OR(0x1c8, ~0x1f, 0xd);

	udelay(1);
	MCHBAR8_AND(0x190, ~1);
	udelay(1); /* 533ns */
	MCHBAR32_AND(0x198, ~0x11554000);
	udelay(1);
	MCHBAR32_AND(0x198, ~0x1455);
	udelay(1);
	MCHBAR8_AND(0x583, ~0x1c);
	MCHBAR8_AND(0x983, ~0x1c);
	udelay(1); /* 533ns */
	MCHBAR8_AND(0x583, ~0x3);
	MCHBAR8_AND(0x983, ~0x3);
	udelay(1); /* 533ns */

	/* ME related */
	MCHBAR32_AND_OR(0x1a0, ~0x7ffffff,
		s->spd_type == DDR2 ? 0x551803 : 0x555801);

	MCHBAR16_AND(0x1b4, ~0x800);
	if (s->spd_type == DDR2) {
		MCHBAR8_OR(0x1a8, 0xf0);
	} else { /* DDR3 */
		reg8 = 0x9; /* 0x9 << 4 ?? */
		if (s->dimms[0].ranks == 2)
			reg8 &= ~0x80;
		if (s->dimms[3].ranks == 2)
			reg8 &= ~0x10;
		MCHBAR8_AND_OR(0x1a8, ~0xf0, reg8);
	}

	FOR_EACH_CHANNEL(i) {
		reg16 = 0;
		if ((s->spd_type == DDR3) && (i == 0))
			reg16 = (0x3 << 12);
		MCHBAR16_AND_OR(0x400*i + 0x59c, ~0x3000, reg16);

		reg32 = 0;
		FOR_EACH_RANK_IN_CHANNEL(r) {
			if (!RANK_IS_POPULATED(s->dimms, i, r))
				reg32 |= 0x111 << r;
		}

		MCHBAR32_AND_OR(0x400*i + 0x59c, ~0xfff, reg32);
		MCHBAR8_AND(0x400*i + 0x594, ~1);

		if (s->spd_type == DDR2) {
			if (!CHANNEL_IS_POPULATED(s->dimms, i)) {
				printk(BIOS_DEBUG,
					"No dimms in channel %d\n", i);
				reg8 = 0x3f;
			} else if (ONLY_DIMMA_IS_POPULATED(s->dimms, i)) {
				printk(BIOS_DEBUG,
					"DimmA populated only in channel %d\n",
					i);
				reg8 = 0x38;
			} else if (ONLY_DIMMB_IS_POPULATED(s->dimms, i)) {
				printk(BIOS_DEBUG,
					"DimmB populated only in channel %d\n",
					i);
				reg8 =  0x7;
			} else if (BOTH_DIMMS_ARE_POPULATED(s->dimms, i)) {
				printk(BIOS_DEBUG,
					"Both dimms populated in channel %d\n",
					i);
				reg8 = 0;
			} else {
				die("Unhandled case\n");
			}
			MCHBAR32_AND_OR(0x400*i + 0x5a0, ~0x3f000000,
				(u32)(reg8 << 24));

		} else { /* DDR3 */
			FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, i, r) {
				MCHBAR8_AND(0x400 * i + 0x5a0 + 3,
					~rank2clken[r + i * 4]);
			}
		}
	} /* END EACH CHANNEL */

	if (s->spd_type == DDR2) {
		MCHBAR8_OR(0x1a8, 1);
		MCHBAR8_AND(0x1a8, ~0x4);
	} else { /* DDR3 */
		MCHBAR8_AND(0x1a8, ~1);
		MCHBAR8_OR(0x1a8, 0x4);
	}

	/* Update DLL timing */
	MCHBAR8_AND(0x1a4, ~0x80);
	MCHBAR8_OR(0x1a4, 0x40);
	MCHBAR16_AND_OR(0x5f0, ~0x400, 0x400);

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		MCHBAR16_AND_OR(0x400*i + 0x5f0, ~0x3fc, 0x3fc);
		MCHBAR32_AND(0x400*i + 0x5fc, ~0xcccccccc);
		MCHBAR8_AND_OR(0x400*i + 0x5d9, ~0xf0,
			s->spd_type == DDR2 ? 0x70 : 0x60);
		MCHBAR16_AND_OR(0x400*i + 0x590, ~0xffff,
			s->spd_type == DDR2 ? 0x5555 : 0xa955);
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		const struct dll_setting *setting;

		switch (s->selected_timings.mem_clk) {
		default: /* Should not happen */
	        case MEM_CLOCK_667MHz:
			setting = default_ddr2_667_ctrl;
			break;
		case MEM_CLOCK_800MHz:
			if (s->spd_type == DDR2)
				setting = default_ddr2_800_ctrl;
			else
				setting = default_ddr3_800_ctrl[s->nmode - 1];
			break;
		case MEM_CLOCK_1066MHz:
			setting = default_ddr3_1067_ctrl[s->nmode - 1];
			break;
		case MEM_CLOCK_1333MHz:
			setting = default_ddr3_1333_ctrl[s->nmode - 1];
			break;
		}

		clkset0(i, &setting[CLKSET0]);
		clkset1(i, &setting[CLKSET1]);
		ctrlset0(i, &setting[CTRL0]);
		ctrlset1(i, &setting[CTRL1]);
		ctrlset2(i, &setting[CTRL2]);
		ctrlset3(i, &setting[CTRL3]);
		cmdset(i, &setting[CMD]);
	}

	/* XXX if not async mode */
	MCHBAR16_AND(0x180, ~0x8200);
	MCHBAR8_OR(0x180, 0x4);
	j = 0;
	for (i = 0; i < 16; i++) {
		MCHBAR8_AND_OR(0x1c8, ~0x1f, i);
		MCHBAR8_OR(0x180, 0x10);
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
			MCHBAR8_AND_OR(0x1c8, ~0x1f, i);
			MCHBAR8_OR(0x180, 0x4);
			while (MCHBAR8(0x180) & 0x10)
				;
			if (MCHBAR32(0x184) == 0) {
				i++;
				break;
			}
		}
		for (; i < 16; i++) {
			MCHBAR8_AND_OR(0x1c8, ~0x1f, i);
			MCHBAR8_OR(0x180, 0x10);
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
			MCHBAR8_AND(0x1c8, ~0x1f);
			MCHBAR8_OR(0x180, 0x10);
			while (MCHBAR8(0x180) & 0x10)
				;
			j = 2;
		}
	}

	if (j < 2) {
		MCHBAR8_AND(0x1c8, ~0x1f);
		async = 1;
		printk(BIOS_NOTICE, "HMC failed, using async mode\n");
	}

	switch (s->selected_timings.mem_clk) {
	case MEM_CLOCK_667MHz:
		clk = 0x1a;
		if (async != 1) {
			if (s->selected_timings.fsb_clk == FSB_CLOCK_800MHz)
				clk = 0x10;
		}
		break;
	case MEM_CLOCK_800MHz:
	case MEM_CLOCK_1066MHz:
		if (async != 1)
			clk = 0x10;
		else
			clk = 0x1a;
		break;
	case MEM_CLOCK_1333MHz:
		clk = 0x18;
		break;
	default:
		clk = 0x1a;
		break;
	}

	MCHBAR8_AND(0x180, ~0x80);

	if ((s->spd_type == DDR3 && s->selected_timings.mem_clk == MEM_CLOCK_1066MHz)
		|| (s->spd_type == DDR2 && s->selected_timings.fsb_clk == FSB_CLOCK_800MHz
			&& s->selected_timings.mem_clk == MEM_CLOCK_667MHz)) {
		i = MCHBAR8(0x1c8) & 0xf;
		if (s->spd_type == DDR2)
			i = (i + 10) % 14;
		else /* DDR3 */
			i = (i + 3) % 12;
		MCHBAR8_AND_OR(0x1c8, ~0x1f, i);
		MCHBAR8_OR(0x180, 0x10);
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
		MCHBAR8_OR(0x18c, 1);
}

static void select_default_dq_dqs_settings(struct sysinfo *s)
{
	int ch, lane;

	FOR_EACH_POPULATED_CHANNEL_AND_BYTELANE(s->dimms, ch, lane) {
		switch (s->selected_timings.mem_clk) {
		case MEM_CLOCK_667MHz:
			memcpy(s->dqs_settings[ch],
				default_ddr2_667_dqs,
				sizeof(s->dqs_settings[ch]));
			memcpy(s->dq_settings[ch],
				default_ddr2_667_dq,
				sizeof(s->dq_settings[ch]));
			s->rt_dqs[ch][lane].tap = 7;
			s->rt_dqs[ch][lane].pi = 2;
			break;
		case MEM_CLOCK_800MHz:
			if (s->spd_type == DDR2) {
				memcpy(s->dqs_settings[ch],
					default_ddr2_800_dqs,
					sizeof(s->dqs_settings[ch]));
				memcpy(s->dq_settings[ch],
					default_ddr2_800_dq,
					sizeof(s->dq_settings[ch]));
				s->rt_dqs[ch][lane].tap = 7;
				s->rt_dqs[ch][lane].pi = 0;
			} else { /* DDR3 */
				memcpy(s->dqs_settings[ch],
					default_ddr3_800_dqs[s->nmode - 1],
					sizeof(s->dqs_settings[ch]));
				memcpy(s->dq_settings[ch],
					default_ddr3_800_dq[s->nmode - 1],
					sizeof(s->dq_settings[ch]));
				s->rt_dqs[ch][lane].tap = 6;
				s->rt_dqs[ch][lane].pi = 3;
			}
			break;
		case MEM_CLOCK_1066MHz:
			memcpy(s->dqs_settings[ch],
				default_ddr3_1067_dqs[s->nmode - 1],
				sizeof(s->dqs_settings[ch]));
			memcpy(s->dq_settings[ch],
				default_ddr3_1067_dq[s->nmode - 1],
				sizeof(s->dq_settings[ch]));
			s->rt_dqs[ch][lane].tap = 5;
			s->rt_dqs[ch][lane].pi = 3;
			break;
		case MEM_CLOCK_1333MHz:
			memcpy(s->dqs_settings[ch],
				default_ddr3_1333_dqs[s->nmode - 1],
				sizeof(s->dqs_settings[ch]));
			memcpy(s->dq_settings[ch],
				default_ddr3_1333_dq[s->nmode - 1],
				sizeof(s->dq_settings[ch]));
			s->rt_dqs[ch][lane].tap = 7;
			s->rt_dqs[ch][lane].pi = 0;
			break;
		default: /* not supported */
			break;
		}
	}
}

/*
 * It looks like only the RT DQS register for the first rank
 * is used for all ranks. Just set all the 'unused' RT DQS registers
 * to the same as rank 0, out of precaution.
 */
static void set_all_dq_dqs_dll_settings(struct sysinfo *s)
{
	/* Program DQ/DQS dll settings */
	int ch, lane, rank;

	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		FOR_EACH_BYTELANE(lane) {
			FOR_EACH_RANK_IN_CHANNEL(rank) {
				rt_set_dqs(ch, lane, rank,
					&s->rt_dqs[ch][lane]);
			}
			dqsset(ch, lane, &s->dqs_settings[ch][lane]);
			dqset(ch, lane, &s->dq_settings[ch][lane]);
		}
	}
}

static void prog_rcomp(struct sysinfo *s)
{
	u8 i, j, k, reg8;
	const u32 ddr2_x32a[8] = { 0x04040404, 0x06050505, 0x09090807, 0x0D0C0B0A,
			0x04040404, 0x08070605, 0x0C0B0A09, 0x100F0E0D };
	const u16 ddr2_x378[5] = { 0xAAAA, 0x7777, 0x7777, 0x7777, 0x7777 };
	const u32 ddr2_x382[5] = { 0x02020202, 0x02020202, 0x02020202, 0x04030303, 0x04030303 };
	const u32 ddr2_x386[5] = { 0x03020202, 0x03020202, 0x03020202, 0x05040404, 0x05040404 };
	const u32 ddr2_x38a[5] = { 0x04040303, 0x04040303, 0x04040303, 0x07070605, 0x07070605 };
	const u32 ddr2_x38e[5] = { 0x06060505, 0x06060505, 0x06060505, 0x09090808, 0x09090808 };
	const u32 ddr2_x392[5] = { 0x02020202, 0x02020202, 0x02020202, 0x03030202, 0x03030202 };
	const u32 ddr2_x396[5] = { 0x03030202, 0x03030202, 0x03030202, 0x05040303, 0x05040303 };
	const u32 ddr2_x39a[5] = { 0x04040403, 0x04040403, 0x04040403, 0x07070605, 0x07070605 };
	const u32 ddr2_x39e[5] = { 0x06060505, 0x06060505, 0x06060505, 0x08080808, 0x08080808 };

	const u32 ddr3_x32a[8] = {0x06060606, 0x06060606, 0x0b090807, 0x12110f0d,
				 0x06060606, 0x08070606, 0x0d0b0a09, 0x16161511};
	const u16 ddr3_x378[5] = {0xbbbb, 0x6666, 0x6666, 0x6666, 0x6666};
	const u32 ddr3_x382[5] = {0x05050505, 0x04040404, 0x04040404, 0x34343434, 0x34343434};
	const u32 ddr3_x386[5] = {0x05050505, 0x04040404, 0x04040404, 0x34343434, 0x34343434};
	const u32 ddr3_x38a[5] = {0x06060605, 0x07060504, 0x07060504, 0x34343434, 0x34343434};
	const u32 ddr3_x38e[5] = {0x09080707, 0x09090808, 0x09090808, 0x34343434, 0x34343434};
	const u32 ddr3_x392[5] = {0x05050505, 0x04040404, 0x04040404, 0x34343434, 0x34343434};
	const u32 ddr3_x396[5] = {0x05050505, 0x04040404, 0x04040404, 0x34343434, 0x34343434};
	const u32 ddr3_x39a[5] = {0x07060606, 0x08070605, 0x08070605, 0x34343434, 0x34343434};
	const u32 ddr3_x39e[5] = {0x09090807, 0x0b0b0a09, 0x0b0b0a09, 0x34343434, 0x34343434};

	const u16 *x378;
	const u32 *x32a, *x382, *x386, *x38a, *x38e;
	const u32 *x392, *x396, *x39a, *x39e;

	const u16 addr[5] = { 0x374, 0x3a2, 0x3d0, 0x3fe, 0x42c };
	u8 bit[5] = { 0, 1, 1, 0, 0 };

	if (s->spd_type == DDR2) {
		x32a = ddr2_x32a;
		x378 = ddr2_x378;
		x382 = ddr2_x382;
		x386 = ddr2_x386;
		x38a = ddr2_x38a;
		x38e = ddr2_x38e;
		x392 = ddr2_x392;
		x396 = ddr2_x396;
		x39a = ddr2_x39a;
		x39e = ddr2_x39e;
	} else { /* DDR3 */
		x32a = ddr3_x32a;
		x378 = ddr3_x378;
		x382 = ddr3_x382;
		x386 = ddr3_x386;
		x38a = ddr3_x38a;
		x38e = ddr3_x38e;
		x392 = ddr3_x392;
		x396 = ddr3_x396;
		x39a = ddr3_x39a;
		x39e = ddr3_x39e;
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		/* RCOMP data group is special, program it separately */
		MCHBAR32_AND_OR(0x400*i + 0x31c, ~0xff000,
			0xaa000);
		MCHBAR16_AND_OR(0x400*i + 0x320, ~0xffff,
			0x6666);
		for (k = 0; k < 8; k++) {
			MCHBAR32_AND_OR(0x400*i + 0x31c +
				0xe + (k << 2),
				~0x3f3f3f3f, x32a[k]);
			MCHBAR32_AND_OR(0x400*i + 0x31c +
				0x2e + (k << 2),
				~0x3f3f3f3f, x32a[k]);
		}
		MCHBAR8_AND_OR(0x400*i + 0x31c, ~1, 0);

		/* Now program the other RCOMP groups */
		for (j = 0; j < ARRAY_SIZE(addr); j++) {
			{
				MCHBAR16_AND_OR(0x400*i + addr[j],
					~0xf000, 0xa000);
				MCHBAR16_AND_OR(0x400*i + addr[j] + 4,
					~0xffff, x378[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0xe,
					~0x3f3f3f3f, x382[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0x12,
					~0x3f3f3f3f, x386[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0x16,
					~0x3f3f3f3f, x38a[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0x1a,
					~0x3f3f3f3f, x38e[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0x1e,
					~0x3f3f3f3f, x392[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0x22,
					~0x3f3f3f3f, x396[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0x26,
					~0x3f3f3f3f, x39a[j]);
				MCHBAR32_AND_OR(0x400*i + addr[j] + 0x2a,
					~0x3f3f3f3f, x39e[j]);
			}

			/* Override command group strength multiplier */
			if (s->spd_type == DDR3 &&
				BOTH_DIMMS_ARE_POPULATED(s->dimms, i)) {
				MCHBAR16_AND_OR(0x378 + 0x400 * i,
					~0xffff, 0xcccc);
			}
			MCHBAR8_AND_OR(0x400*i + addr[j], ~1, bit[j]);
		}
		reg8 = (s->spd_type == DDR2) ? 0x12 : 0x36;
		MCHBAR8_AND_OR(0x400*i + 0x45a, ~0x3f, reg8);
		MCHBAR8_AND_OR(0x400*i + 0x45e, ~0x3f, reg8);
		MCHBAR8_AND_OR(0x400*i + 0x462, ~0x3f, reg8);
		MCHBAR8_AND_OR(0x400*i + 0x466, ~0x3f, reg8);
	} /* END EACH POPULATED CHANNEL */

	MCHBAR32_AND_OR(0x134, ~0x63c00, 0x63c00);
	MCHBAR16_AND_OR(0x174, ~0x63ff, 0x63ff);
	MCHBAR16(0x178) = 0x0135;
	MCHBAR32_AND_OR(0x130, ~0x7bdffe0, 0x7a9ffa0);

	if (!CHANNEL_IS_POPULATED(s->dimms, 0))
		MCHBAR32_AND(0x130, ~(1 << 27));
	if (!CHANNEL_IS_POPULATED(s->dimms, 1))
		MCHBAR32_AND(0x130, ~(1 << 28));

	MCHBAR8_OR(0x130, 1);
}

static void program_odt(struct sysinfo *s)
{
	u8 i;
	static u16 ddr2_odt[16][2] = {
		{ 0x0000, 0x0000 }, /* NC_NC */
		{ 0x0000, 0x0001 }, /* x8SS_NC */
		{ 0x0000, 0x0011 }, /* x8DS_NC */
		{ 0x0000, 0x0001 }, /* x16SS_NC */
		{ 0x0004, 0x0000 }, /* NC_x8SS */
		{ 0x0101, 0x0404 }, /* x8SS_x8SS */
		{ 0x0101, 0x4444 }, /* x8DS_x8SS */
		{ 0x0101, 0x0404 }, /* x16SS_x8SS */
		{ 0x0044, 0x0000 }, /* NC_x8DS */
		{ 0x1111, 0x0404 }, /* x8SS_x8DS */
		{ 0x1111, 0x4444 }, /* x8DS_x8DS */
		{ 0x1111, 0x0404 }, /* x16SS_x8DS */
		{ 0x0004, 0x0000 }, /* NC_x16SS */
		{ 0x0101, 0x0404 }, /* x8SS_x16SS */
		{ 0x0101, 0x4444 }, /* x8DS_x16SS */
		{ 0x0101, 0x0404 }, /* x16SS_x16SS */
	};

	static const u16 ddr3_odt[16][2] = {
		{ 0x0000, 0x0000 }, /* NC_NC */
		{ 0x0000, 0x0001 }, /* x8SS_NC */
		{ 0x0000, 0x0021 }, /* x8DS_NC */
		{ 0x0000, 0x0001 }, /* x16SS_NC */
		{ 0x0004, 0x0000 }, /* NC_x8SS */
		{ 0x0105, 0x0405 }, /* x8SS_x8SS */
		{ 0x0105, 0x4465 }, /* x8DS_x8SS */
		{ 0x0105, 0x0405 }, /* x16SS_x8SS */
		{ 0x0084, 0x0000 }, /* NC_x8DS */
		{ 0x1195, 0x0405 }, /* x8SS_x8DS */
		{ 0x1195, 0x4465 }, /* x8DS_x8DS */
		{ 0x1195, 0x0405 }, /* x16SS_x8DS */
		{ 0x0004, 0x0000 }, /* NC_x16SS */
		{ 0x0105, 0x0405 }, /* x8SS_x16SS */
		{ 0x0105, 0x4465 }, /* x8DS_x16SS */
		{ 0x0105, 0x0405 }, /* x16SS_x16SS */
	};

	FOR_EACH_POPULATED_CHANNEL(s->dimms, i) {
		if (s->spd_type == DDR2) {
			MCHBAR16(0x400 * i + 0x298) =
				ddr2_odt[s->dimm_config[i]][1];
			MCHBAR16(0x400 * i + 0x294) =
				ddr2_odt[s->dimm_config[i]][0];
		} else {
			MCHBAR16(0x400 * i + 0x298) =
				ddr3_odt[s->dimm_config[i]][1];
			MCHBAR16(0x400 * i + 0x294) =
				ddr3_odt[s->dimm_config[i]][0];
		}
		u16 reg16 = MCHBAR16(0x400*i + 0x29c);
		reg16 &= ~0xfff;
		reg16 |= (s->spd_type == DDR2 ? 0x66b : 0x778);
		MCHBAR16(0x400*i + 0x29c) = reg16;
		MCHBAR32_AND_OR(0x400*i + 0x260, ~0x70e3c00, 0x3063c00);
	}
}

static void pre_jedec_memory_map(void)
{
	/*
	 * Configure the memory mapping in stacked mode (channel 1 being mapped
	 * above channel 0) and with 128M per rank.
	 * This simplifies dram trainings a lot since those need a test address.
	 *
	 * +-------------+ => 0
	 * | ch 0, rank 0|
	 * +-------------+ => 0x8000000 (128M)
	 * | ch 0, rank 1|
	 * +-------------+ => 0x10000000 (256M)
	 * | ch 0, rank 2|
	 * +-------------+ => 0x18000000 (384M)
	 * | ch 0, rank 3|
	 * +-------------+ => 0x20000000 (512M)
	 * | ch 1, rank 0|
	 * +-------------+ => 0x28000000 (640M)
	 * | ch 1, rank 1|
	 * +-------------+ => 0x30000000 (768M)
	 * | ch 1, rank 2|
	 * +-------------+ => 0x38000000 (896M)
	 * | ch 1, rank 3|
	 * +-------------+
	 *
	 * After all trainings are done this is set to the real values specified
	 * by the SPD.
	 */
	/* Set rank 0-3 populated */
	MCHBAR32_AND_OR(C0CKECTRL, ~1, 0xf00000);
	MCHBAR32_AND_OR(C1CKECTRL, ~1, 0xf00000);
	/* Set size of each rank to 128M */
	MCHBAR16(C0DRA01) = 0x0101;
	MCHBAR16(C0DRA23) = 0x0101;
	MCHBAR16(C1DRA01) = 0x0101;
	MCHBAR16(C1DRA23) = 0x0101;
	MCHBAR16(C0DRB0) = 0x0002;
	MCHBAR16(C0DRB1) = 0x0004;
	MCHBAR16(C0DRB2) = 0x0006;
	MCHBAR16(C0DRB3) = 0x0008;
	MCHBAR16(C1DRB0) = 0x0002;
	MCHBAR16(C1DRB1) = 0x0004;
	MCHBAR16(C1DRB2) = 0x0006;
	/* In stacked mode the last present rank on ch1 needs to have its
	   size doubled in c1drbx */
	MCHBAR16(C1DRB3) = 0x0010;
	MCHBAR8_OR(0x111, STACKED_MEM);
	MCHBAR32(0x104) = 0;
	MCHBAR16(0x102) = 0x400;
	MCHBAR8(0x110) = (2 << 5) | (3 << 3);
	MCHBAR16(0x10e) = 0;
	MCHBAR32(0x108) = 0;
	pci_write_config16(HOST_BRIDGE, D0F0_TOLUD, 0x4000);
	/* TOM(64M unit) = 1G = TOTAL_CHANNELS * RANKS_PER_CHANNEL * 128M */
	pci_write_config16(HOST_BRIDGE, D0F0_TOM, 0x10);
	/* TOUUD(1M unit) = 1G = TOTAL_CHANNELS * RANKS_PER_CHANNEL * 128M */
	pci_write_config16(HOST_BRIDGE, D0F0_TOUUD, 0x0400);
	pci_write_config32(HOST_BRIDGE, D0F0_GBSM, 0x40000000);
	pci_write_config32(HOST_BRIDGE, D0F0_BGSM, 0x40000000);
	pci_write_config32(HOST_BRIDGE, D0F0_TSEG, 0x40000000);
}

u32 test_address(int channel, int rank)
{
	ASSERT(channel <= 1 && rank < 4);
	return channel * 512 * MiB + rank * 128 * MiB;
}

/* DDR3 Rank1 Address mirror
   swap the following pins:
   A3<->A4, A5<->A6, A7<->A8, BA0<->BA1 */
static u32 mirror_shift_bit(const u32 data, u8 bit)
{
	u32 temp0 = data, temp1 = data;
	temp0 &= 1 << bit;
	temp0 <<= 1;
	temp1 &= 1 << (bit + 1);
	temp1 >>= 1;
	return (data & ~(3 << bit)) | temp0 | temp1;
}

void send_jedec_cmd(const struct sysinfo *s, u8 r, u8 ch, u8 cmd, u32 val)
{
	u32 addr = test_address(ch, r);
	u8 data8 = cmd;
	u32 data32;

	if (s->spd_type == DDR3 && (r & 1)
			&& s->dimms[ch * 2 + (r >> 1)].mirrored) {
		data8 = (u8)mirror_shift_bit(data8, 4);
	}

	MCHBAR8_AND_OR(0x271, ~0x3e, data8);
	MCHBAR8_AND_OR(0x671, ~0x3e, data8);
	data32 = val;
	if (s->spd_type == DDR3 && (r & 1)
			&& s->dimms[ch * 2 + (r >> 1)].mirrored) {
		data32 = mirror_shift_bit(data32, 3);
		data32 = mirror_shift_bit(data32, 5);
		data32 = mirror_shift_bit(data32, 7);
	}
	data32 <<= 3;

	read32((void *)((data32 | addr)));
	udelay(10);
	MCHBAR8_AND_OR(0x271, ~0x3e, NORMALOP_CMD);
	MCHBAR8_AND_OR(0x671, ~0x3e, NORMALOP_CMD);
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
		{MRS_CMD, 0x100},	/* DLL Reset */
		{PRECHARGE_CMD, 0x0},
		{CBR_CMD, 0x0},
		{CBR_CMD, 0x0},
		{MRS_CMD, 0x0},		/* DLL out of reset */
		{EMRS1_CMD, 0x380},	/* OCD calib default */
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
			send_jedec_cmd(s, r, ch, jedec[i][0], v);
			udelay(1);
			printk(RAM_SPEW, "Jedec step %d\n", i);
		}
	}
	printk(BIOS_DEBUG, "MRS done\n");
}

static void jedec_ddr3(struct sysinfo *s)
{
	int ch, r, dimmconfig, cmd, ddr3_freq;

	u8 ddr3_emrs2_rtt_wr_config[16][4] = { /* [config][Rank] */
		{0, 0, 0, 0},	/* NC_NC */
		{0, 0, 0, 0},	/* x8ss_NC */
		{0, 0, 0, 0},	/* x8ds_NC */
		{0, 0, 0, 0},	/* x16ss_NC */
		{0, 0, 0, 0},	/* NC_x8ss */
		{2, 0, 2, 0},	/* x8ss_x8ss */
		{2, 2, 2, 0},	/* x8ds_x8ss */
		{2, 0, 2, 0},	/* x16ss_x8ss */
		{0, 0, 0, 0},	/* NC_x8ss */
		{2, 0, 2, 2},	/* x8ss_x8ds */
		{2, 2, 2, 2},	/* x8ds_x8ds */
		{2, 0, 2, 2},	/* x16ss_x8ds */
		{0, 0, 0, 0},	/* NC_x16ss */
		{2, 0, 2, 0},	/* x8ss_x16ss */
		{2, 2, 2, 0},	/* x8ds_x16ss */
		{2, 0, 2, 0},	/* x16ss_x16ss */
	};

	printk(BIOS_DEBUG, "MRS...\n");

	ddr3_freq = s->selected_timings.mem_clk - MEM_CLOCK_800MHz;
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		printk(BIOS_DEBUG, "CH%d: Found Rank %d\n", ch, r);
		send_jedec_cmd(s, r, ch, NOP_CMD, 0);
		udelay(200);
		dimmconfig = s->dimm_config[ch];
		cmd = ddr3_freq << 3; /* actually twl - 5 which is same */
		cmd |= ddr3_emrs2_rtt_wr_config[dimmconfig][r] << 9;
		send_jedec_cmd(s, r, ch, EMRS2_CMD, cmd);
		send_jedec_cmd(s, r, ch, EMRS3_CMD, 0);
		cmd = ddr3_emrs1_rtt_nom_config[dimmconfig][r] << 2;
		/* Hardcode output drive strength to 34 Ohm / RZQ/7 (why??) */
		cmd |= (1 << 1);
		send_jedec_cmd(s, r, ch, EMRS1_CMD, cmd);
		/* Burst type interleaved, burst length 8, Reset DLL,
		   Precharge PD: DLL on */
		send_jedec_cmd(s, r, ch, MRS_CMD, (1 << 3) | (1 << 8)
			| (1 << 12) | ((s->selected_timings.CAS - 4) << 4)
			| ((s->selected_timings.tWR - 4) << 9));
		send_jedec_cmd(s, r, ch, ZQCAL_CMD, (1 << 10));
	}
	printk(BIOS_DEBUG, "MRS done\n");
}

static void sdram_recover_receive_enable(const struct sysinfo *s)
{
	u32 reg32;
	u16 medium, coarse_offset;
	u8 pi_tap;
	int lane, channel;

	FOR_EACH_POPULATED_CHANNEL(s->dimms, channel) {
		medium = 0;
		coarse_offset = 0;
		reg32 = MCHBAR32(0x400 * channel + 0x248);
		reg32 &= ~0xf0000;
		reg32 |= s->rcven_t[channel].min_common_coarse << 16;
		MCHBAR32(0x400 * channel + 0x248) = reg32;

	        FOR_EACH_BYTELANE(lane) {
			medium |= s->rcven_t[channel].medium[lane]
				<< (lane * 2);
			coarse_offset |=
				(s->rcven_t[channel].coarse_offset[lane] & 0x3)
				<< (lane * 2);

			pi_tap = MCHBAR8(0x400 * channel + 0x560 + lane * 4);
			pi_tap &= ~0x7f;
			pi_tap |= s->rcven_t[channel].tap[lane];
			pi_tap |= s->rcven_t[channel].pi[lane] << 4;
			MCHBAR8(0x400 * channel + 0x560 + lane * 4) = pi_tap;
		}
		MCHBAR16(0x400 * channel + 0x58c) = medium;
		MCHBAR16(0x400 * channel + 0x5fa) = coarse_offset;
	}
}

static void sdram_program_receive_enable(struct sysinfo *s, int fast_boot)
{
	/* Program Receive Enable Timings */
	if (fast_boot)
		sdram_recover_receive_enable(s);
	else
		rcven(s);
}

static void set_dradrb(struct sysinfo *s)
{
	u8 map, i, ch, r, rankpop0, rankpop1, lastrank_ch1;
	u32 c0dra = 0;
	u32 c1dra = 0;
	u32 c0drb = 0;
	u32 c1drb = 0;
	u32 dra;
	u32 dra0;
	u32 dra1;
	u16 totalmemorymb;
	u32 dual_channel_size, single_channel_size, single_channel_offset;
	u32 size_ch0, size_ch1, size_me;
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

	/* DRA */
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

	MCHBAR8_AND_OR(0x262, ~0xf0, (rankpop0 << 4) & 0xf0);
	MCHBAR8_AND_OR(0x662, ~0xf0, (rankpop1 << 4) & 0xf0);

	if (s->spd_type == DDR3) {
		FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
			/* ZQCAL enable */
			MCHBAR32_OR(0x269 + 0x400 * ch, 1 << 26);
		}
	}

	if (ONLY_DIMMA_IS_POPULATED(s->dimms, 0) ||
			ONLY_DIMMB_IS_POPULATED(s->dimms, 0))
		MCHBAR8_OR(0x260, 1);
	if (ONLY_DIMMA_IS_POPULATED(s->dimms, 1) ||
			ONLY_DIMMB_IS_POPULATED(s->dimms, 1))
		MCHBAR8_OR(0x660, 1);

	/* DRB */
	lastrank_ch1 = 0;
	FOR_EACH_RANK(ch, r) {
		if (ch == 0) {
			if (RANK_IS_POPULATED(s->dimms, ch, r)) {
				dra0 = (c0dra >> (8*r)) & 0x7f;
				c0drb = (u16)(c0drb + drbtab[dra0]);
			}
			MCHBAR16(0x200 + 2*r) = c0drb;
		} else {
			if (RANK_IS_POPULATED(s->dimms, ch, r)) {
				lastrank_ch1 = r;
				dra1 = (c1dra >> (8*r)) & 0x7f;
				c1drb = (u16)(c1drb + drbtab[dra1]);
			}
			MCHBAR16(0x600 + 2*r) = c1drb;
		}
	}

	s->channel_capacity[0] = c0drb << 6;
	s->channel_capacity[1] = c1drb << 6;

	/*
	 * In stacked mode the last present rank on ch1 needs to have its
	 * size doubled in c1drbx. All subsequent ranks need the same setting
	 * according to: "Intel 4 Series Chipset Family Datasheet"
	 */
	if (s->stacked_mode) {
		for (r = lastrank_ch1; r < 4; r++)
			MCHBAR16(0x600 + 2*r) = 2 * c1drb;
	}

	totalmemorymb = s->channel_capacity[0] + s->channel_capacity[1];
	printk(BIOS_DEBUG, "Total memory: %d + %d = %dMiB\n",
		s->channel_capacity[0], s->channel_capacity[1], totalmemorymb);

	/* Populated channel sizes in MiB */
	size_ch0 = s->channel_capacity[0];
	size_ch1 = s->channel_capacity[1];
	size_me = ME_UMA_SIZEMB;

	if (s->stacked_mode) {
		MCHBAR8_OR(0x111, STACKED_MEM);
	} else {
		MCHBAR8_AND(0x111, ~STACKED_MEM);
		MCHBAR8_OR(0x111, 1 << 4);
	}

	if (s->stacked_mode) {
		dual_channel_size = 0;
	} else if (size_me == 0) {
		dual_channel_size = MIN(size_ch0, size_ch1) * 2;
	} else {
		if (size_ch0 == 0) {
			/* ME needs RAM on CH0 */
			size_me = 0;
			/* TOTEST: bailout? */
		} else {
			/* Set ME UMA size in MiB */
			MCHBAR16(0x100) = size_me;
			/* Set ME UMA Present bit */
			MCHBAR32_OR(0x111, 1);
		}
		dual_channel_size = MIN(size_ch0 - size_me, size_ch1) * 2;
	}

	MCHBAR16(0x104) = dual_channel_size;
	single_channel_size = size_ch0 + size_ch1 - dual_channel_size;
	MCHBAR16(0x102) = single_channel_size;

	map = 0;
	if (size_ch0 == 0)
		map = 0;
	else if (size_ch1 == 0)
		map |= 0x20;
	else
		map |= 0x40;

	if (dual_channel_size == 0)
		map |= 0x18;
	/* Enable flex mode, we hardcode this everywhere */
	if (size_me == 0) {
		if (!(s->stacked_mode && size_ch0 != 0 && size_ch1 != 0)) {
			map |= 0x04;
			if (size_ch0 <= size_ch1)
				map |= 0x01;
		}
	} else {
		if (s->stacked_mode == 0 && size_ch0 - size_me < size_ch1)
			map |= 0x04;
	}

	MCHBAR8(0x110) = map;
	MCHBAR16(0x10e) = 0;

	/*
	 * "108h[15:0] Single Channel Offset for Ch0"
	 * This is the 'limit' of the part on CH0 that cannot be matched
	 * with memory on CH1. MCHBAR16(0x10a) is where the dual channel
	 * memory on ch0s end and MCHBAR16(0x108) is the limit of the single
	 * channel size on ch0.
	 */
	if (s->stacked_mode && size_ch1 != 0) {
		single_channel_offset = 0;
	} else if (size_me == 0) {
		if (size_ch0 > size_ch1)
			single_channel_offset = dual_channel_size / 2
				+ single_channel_size;
		else
			single_channel_offset = dual_channel_size / 2;
	} else {
		if ((size_ch0 > size_ch1) && ((map & 0x7) == 4))
			single_channel_offset = dual_channel_size / 2
				+ single_channel_size;
		else
			single_channel_offset = dual_channel_size / 2
				+ size_me;
	}

	MCHBAR16(0x108) = single_channel_offset;
	MCHBAR16(0x10a) = dual_channel_size / 2;
}

static void configure_mmap(struct sysinfo *s)
{
	bool reclaim;
	u32 gfxsize, gttsize, tsegsize, mmiosize, tom, tolud, touud;
	u32 gfxbase, gttbase, tsegbase, reclaimbase, reclaimlimit;
	u32 mmiostart, umasizem;
	u16 ggc;
	u16 ggc2uma[] = { 0, 1, 4, 8, 16, 32, 48, 64, 128, 256, 96,
			  160, 224, 352 };
	u8 ggc2gtt[] = { 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4};

	ggc = pci_read_config16(HOST_BRIDGE, 0x52);
	gfxsize = ggc2uma[(ggc & 0xf0) >> 4];
	gttsize = ggc2gtt[(ggc & 0xf00) >> 8];
	/* TSEG 2M, This amount can easily be covered by SMRR MTRR's,
	   which requires to have TSEG_BASE aligned to TSEG_SIZE. */
	tsegsize = 2;
	mmiosize = 0x800; /* 2GB MMIO */
	umasizem = gfxsize + gttsize + tsegsize;
	mmiostart = 0x1000 - mmiosize + umasizem;
	tom = s->channel_capacity[0] + s->channel_capacity[1] - ME_UMA_SIZEMB;
	tolud = MIN(mmiostart, tom);

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

	pci_write_config16(HOST_BRIDGE, 0xb0, tolud << 4);
	pci_write_config16(HOST_BRIDGE, 0xa0, tom >> 6);
	if (reclaim) {
		pci_write_config16(HOST_BRIDGE, 0x98,
					(u16)(reclaimbase >> 6));
		pci_write_config16(HOST_BRIDGE, 0x9a,
					(u16)(reclaimlimit >> 6));
	}
	pci_write_config16(HOST_BRIDGE, 0xa2, touud);
	pci_write_config32(HOST_BRIDGE, 0xa4, gfxbase << 20);
	pci_write_config32(HOST_BRIDGE, 0xa8, gttbase << 20);
	/* Enable and set TSEG size to 2M */
	pci_update_config8(HOST_BRIDGE, D0F0_ESMRAMC, ~0x07, (1 << 1) | (1 << 0));
	pci_write_config32(HOST_BRIDGE, 0xac, tsegbase << 20);
}

static void set_enhanced_mode(struct sysinfo *s)
{
	u8 ch, reg8;
	u32 reg32;

	MCHBAR32(0xfb0) = 0x1000d024;
	MCHBAR32(0xfb4) = 0xc842;
	MCHBAR32(0xfbc) = 0xf;
	MCHBAR32(0xfc4) = 0xfe22244;
	MCHBAR8(0x12f) = 0x5c;
	MCHBAR8_OR(0xfb0, 1);
	if (s->selected_timings.mem_clk <= MEM_CLOCK_800MHz)
		MCHBAR8_OR(0x12f, 0x2);
	else
		MCHBAR8_AND(0x12f, ~0x2);
	MCHBAR8_AND_OR(0x6c0, ~0xf0, 0xa0);
	MCHBAR32(0xfa8) = 0x30d400;

	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR8_OR(0x400*ch + 0x26c, 1);
		MCHBAR32(0x400*ch + 0x278) = 0x88141881;
		MCHBAR16(0x400*ch + 0x27c) = 0x0041;
		MCHBAR8(0x400*ch + 0x292) = 0xf2;
		MCHBAR16_OR(0x400*ch + 0x272, 0x100);
		MCHBAR8_AND_OR(0x400*ch + 0x243, ~0x2, 1);
		MCHBAR32(0x400*ch + 0x288) = 0x8040200;
		MCHBAR32(0x400*ch + 0x28c) = 0xff402010;
		MCHBAR32(0x400*ch + 0x290) = 0x4f2091c;
	}

	reg8 = pci_read_config8(HOST_BRIDGE, 0xf0);
	pci_write_config8(HOST_BRIDGE, 0xf0, reg8 | 1);
	MCHBAR32_AND_OR(0xfa0, ~0x20002, 0x2 | (s->selected_timings.fsb_clk ==
		FSB_CLOCK_1333MHz ? 0x20000 : 0));
	reg32 = 0x219100c2;
	if (s->selected_timings.fsb_clk == FSB_CLOCK_1333MHz) {
		reg32 |= 1;
		if (s->selected_timings.mem_clk == MEM_CLOCK_1066MHz)
			reg32 &= ~0x10000;
	} else if (s->selected_timings.fsb_clk == FSB_CLOCK_1066MHz) {
		reg32 &= ~0x10000;
	}
	MCHBAR32_AND_OR(0xfa4, ~0x219100c3, reg32);
	reg32 = 0x44a00;
	switch (s->selected_timings.fsb_clk) {
	case FSB_CLOCK_1333MHz:
		reg32 |= 0x62;
		break;
	case FSB_CLOCK_1066MHz:
		reg32 |= 0x5a;
		break;
	default:
	case FSB_CLOCK_800MHz:
		reg32 |= 0x53;
		break;
	}

	MCHBAR32(0x2c) = reg32;
	MCHBAR32(0x30) = 0x1f5a86;
	MCHBAR32(0x34) = 0x1902810;
	MCHBAR32(0x38) = 0xf7000000;
	reg32 = 0x23014410;
	if (s->selected_timings.fsb_clk > FSB_CLOCK_800MHz)
		reg32 = (reg32 & ~0x2000000) | 0x44000000;
	MCHBAR32(0x3c) = reg32;
	reg32 = 0x8f038000;
	if (s->selected_timings.fsb_clk == FSB_CLOCK_1333MHz)
		reg32 &= ~0x4000000;
	MCHBAR32_AND_OR(0x40, ~0x8f038000, reg32);
	reg32 = 0x00013001;
	if (s->selected_timings.fsb_clk < FSB_CLOCK_1333MHz)
		reg32 |= 0x20000;
	MCHBAR32(0x20) = reg32;
	pci_write_config8(HOST_BRIDGE, 0xf0, reg8 & ~1);
}

static void power_settings(struct sysinfo *s)
{
	u32 reg1, reg2, reg3, reg4, clkgate, x592;
	u8 lane, ch;
	u8 twl = 0;
	u16 x264, x23c;

	if (s->spd_type == DDR2) {
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
	} else { /* DDR3 */
		int ddr3_idx = s->selected_timings.mem_clk - MEM_CLOCK_800MHz;
		int cas_idx = s->selected_timings.CAS - 5;

		twl = s->selected_timings.mem_clk - MEM_CLOCK_800MHz + 5;
		reg1 = ddr3_c2_tab[s->nmode - 1][ddr3_idx][cas_idx][0];
		reg2 = ddr3_c2_tab[s->nmode - 1][ddr3_idx][cas_idx][1];
		reg3 = 0x764;
		reg4 = 0x78c8;
		x264 = ddr3_c2_x264[ddr3_idx][cas_idx];
		x23c = ddr3_c2_x23c[ddr3_idx][cas_idx];
		switch (s->selected_timings.mem_clk) {
		case MEM_CLOCK_800MHz:
		default:
			clkgate = 0x280000;
			break;
		case MEM_CLOCK_1066MHz:
			clkgate = 0x350000;
			break;
		case MEM_CLOCK_1333MHz:
			clkgate = 0xff0000;
			break;
		}
	}

	if (CHANNEL_IS_POPULATED(s->dimms, 0) && CHANNEL_IS_POPULATED(s->dimms, 1))
		MCHBAR32(0x14) = 0x0010461f;
	else
		MCHBAR32(0x14) = 0x0010691f;
	MCHBAR32(0x18) = 0xdf6437f7;
	MCHBAR32(0x1c) = 0x0;
	MCHBAR32_AND_OR(0x24, ~0xe0000000, 0x60000000);
	MCHBAR32_AND_OR(0x44, ~0x1fef0000, 0x6b0000);
	MCHBAR16(0x115) = (u16) reg1;
	MCHBAR32_AND_OR(0x117, ~0xffffff, reg2);
	MCHBAR8(0x124) = 0x7;
	/* not sure if dummy reads are needed */
	MCHBAR16_AND_OR(0x12a, 0, 0x80);
	MCHBAR8_AND_OR(0x12c, 0, 0xa0);
	MCHBAR16_AND(0x174, ~(1 << 15));
	MCHBAR16_AND_OR(0x188, ~0x1f00, 0x1f00);
	MCHBAR8_AND(0x18c, ~0x8);
	MCHBAR8_OR(0x192, 1);
	MCHBAR8_OR(0x193, 0xf);
	MCHBAR16_AND_OR(0x1b4, ~0x480, 0x80);
	MCHBAR16_AND_OR(0x210, ~0x1fff, 0x3f); /* clockgating iii */
	/* non-aligned access: possible bug? */
	MCHBAR32_AND_OR(0x6d1, ~0xff03ff, 0x100 | clkgate);
	MCHBAR8_AND_OR(0x212, ~0x7f, 0x7f);
	MCHBAR32_AND_OR(0x2c0, ~0xffff0, 0xcc5f0);
	MCHBAR8_AND_OR(0x2c4, ~0x70, 0x70);
	/* non-aligned access: possible bug? */
	MCHBAR32_AND_OR(0x2d1, ~0xffffff, 0xff2831); /* clockgating i */
	MCHBAR32(0x2d4) = 0x40453600;
	MCHBAR32(0x300) = 0xc0b0a08;
	MCHBAR32(0x304) = 0x6040201;
	MCHBAR32_AND_OR(0x30c, ~0x43c0f, 0x41405);
	MCHBAR16(0x610) = reg3;
	MCHBAR16(0x612) = reg4;
	MCHBAR32_AND_OR(0x62c, ~0xc000000, 0x4000000);
	MCHBAR32(0xae4) = 0;
	MCHBAR32_AND_OR(0xc00, ~0xf0000, 0x10000);
	MCHBAR32(0xf00) = 0x393a3b3c;
	MCHBAR32(0xf04) = 0x3d3e3f40;
	MCHBAR32(0xf08) = 0x393a3b3c;
	MCHBAR32(0xf0c) = 0x3d3e3f40;
	MCHBAR32_AND(0xf18, ~0xfff00001);
	MCHBAR32(0xf48) = 0xfff0ffe0;
	MCHBAR32(0xf4c) = 0xffc0ff00;
	MCHBAR32(0xf50) = 0xfc00f000;
	MCHBAR32(0xf54) = 0xc0008000;
	MCHBAR32_AND_OR(0xf6c, ~0xffff0000, 0xffff0000);
	MCHBAR32_AND(0xfac, ~0x80000000);
	MCHBAR32_AND(0xfb8, ~0xff000000);
	MCHBAR32_AND_OR(0xfbc, ~0x7f800, 0xf000);
	MCHBAR32(0x1104) = 0x3003232;
	MCHBAR32(0x1108) = 0x74;
	if (s->selected_timings.fsb_clk == FSB_CLOCK_800MHz)
		MCHBAR32(0x110c) = 0xaa;
	else
		MCHBAR32(0x110c) = 0x100;
	MCHBAR32(0x1110) = 0x10810350 & ~0x78;
	MCHBAR32(0x1114) = 0;
	x592 = 0xff;
	if (pci_read_config8(HOST_BRIDGE, 0x8) < 3)
		x592 = ~0x4;

	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR8(0x400*ch + 0x239) = twl + 15;
		MCHBAR16(0x400*ch + 0x23c) = x23c;
		MCHBAR32_AND_OR(0x400*ch + 0x248, ~0x706033, 0x406033);
		MCHBAR32_AND_OR(0x400*ch + 0x260, ~(1 << 16), 1 << 16);
		MCHBAR8(0x400*ch + 0x264) = x264;
		MCHBAR8_AND_OR(0x400*ch + 0x592, ~0x3f, 0x3c & x592);
		MCHBAR8_AND_OR(0x400*ch + 0x593, ~0x1f, 0x1e);
	}

	for (lane = 0; lane < 8; lane++)
		MCHBAR8_AND(0x561 + (lane << 2), ~(1 << 3));
}

static void software_ddr3_reset(struct sysinfo *s)
{
	printk(BIOS_DEBUG, "Software initiated DDR3 reset.\n");
	MCHBAR8_OR(0x1a8, 0x02);
	MCHBAR8_AND(0x5da, ~0x80);
	MCHBAR8_AND(0x1a8, ~0x02);
	MCHBAR8_AND_OR(0x5da, ~0x03, 1);
	udelay(200);
	MCHBAR8_AND(0x1a8, ~0x02);
	MCHBAR8_OR(0x5da, 0x80);
	MCHBAR8_AND(0x5da, ~0x80);
	udelay(500);
	MCHBAR8_OR(0x5da, 0x03);
	MCHBAR8_AND(0x5da, ~0x03);
	/* After write leveling the dram needs to be reset and reinitialised */
	jedec_ddr3(s);
}

void do_raminit(struct sysinfo *s, int fast_boot)
{
	u8 ch;
	u8 r, bank;
	u32 reg32;

	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		/* Clear self refresh */
		MCHBAR32(PMSTS_MCHBAR) = MCHBAR32(PMSTS_MCHBAR)
			| PMSTS_BOTH_SELFREFRESH;

		/* Clear host clk gate reg */
		MCHBAR32_OR(0x1c, 0xffffffff);

		/* Select type */
		if (s->spd_type == DDR2)
			MCHBAR8_AND(0x1a8, ~0x4);
		else
			MCHBAR8_OR(0x1a8, 0x4);

		/* Set frequency */
		MCHBAR32_AND_OR(0xc00, ~0x70,
			(s->selected_timings.mem_clk << 4) | (1 << 10));

		/* Overwrite value if chipset rejects it */
		s->selected_timings.mem_clk = (MCHBAR8(0xc00) & 0x70) >> 4;
		if (s->selected_timings.mem_clk > (s->max_fsb + 3))
			die("Error: DDR is faster than FSB, halt\n");
	}

	/* Program clock crossing */
	program_crossclock(s);
	printk(BIOS_DEBUG, "Done clk crossing\n");

	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		setioclk_dram(s);
		printk(BIOS_DEBUG, "Done I/O clk\n");
	}

	/* Grant to launch */
	launch_dram(s);
	printk(BIOS_DEBUG, "Done launch\n");

	/* Program DRAM timings */
	program_timings(s);
	printk(BIOS_DEBUG, "Done timings\n");

	/* Program DLL */
	program_dll(s);
	if (!fast_boot)
		select_default_dq_dqs_settings(s);
	set_all_dq_dqs_dll_settings(s);

	/* RCOMP */
	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		prog_rcomp(s);
		printk(BIOS_DEBUG, "RCOMP\n");
	}

	/* ODT */
	program_odt(s);
	printk(BIOS_DEBUG, "Done ODT\n");

	/* RCOMP update */
	if (s->boot_path != BOOT_PATH_WARM_RESET) {
		while (MCHBAR8(0x130) & 1)
			;
		printk(BIOS_DEBUG, "Done RCOMP update\n");
	}

	pre_jedec_memory_map();

	/* IOBUFACT */
	if (CHANNEL_IS_POPULATED(s->dimms, 0)) {
		MCHBAR8_AND_OR(0x5dd, ~0x3f, 0x3f);
		MCHBAR8_OR(0x5d8, 0x7);
	}
	if (CHANNEL_IS_POPULATED(s->dimms, 1)) {
		if (pci_read_config8(HOST_BRIDGE, 0x8) < 2) {
			MCHBAR8_AND_OR(0x5dd, ~0x3f, 0x3f);
			MCHBAR8_OR(0x5d8, 1);
		}
		MCHBAR8_OR(0x9dd, 0x3f);
		MCHBAR8_OR(0x9d8, 0x7);
	}

	/* DDR3 reset */
	if ((s->spd_type == DDR3) && (s->boot_path != BOOT_PATH_RESUME)) {
		printk(BIOS_DEBUG, "DDR3 Reset.\n");
		MCHBAR8_AND(0x1a8, ~0x2);
		MCHBAR8_OR(0x5da, 0x80);
		udelay(500);
		MCHBAR8_AND(0x1a8, ~0x2);
		MCHBAR8_AND(0x5da, ~0x80);
		udelay(500);
	}

	/* Pre jedec */
	MCHBAR8_OR(0x40, 0x2);
	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR32_OR(0x400*ch + 0x260, 1 << 27);
	}
	MCHBAR16_OR(0x212, 0xf000);
	MCHBAR16_OR(0x212, 0xf00);
	printk(BIOS_DEBUG, "Done pre-jedec\n");

	/* JEDEC reset */
	if (s->boot_path != BOOT_PATH_RESUME) {
		if (s->spd_type == DDR2)
			jedec_ddr2(s);
		else /* DDR3 */
			jedec_ddr3(s);
	}

	printk(BIOS_DEBUG, "Done jedec steps\n");

	if (s->spd_type == DDR3) {
		if (!fast_boot)
			search_write_leveling(s);
		if (s->boot_path == BOOT_PATH_NORMAL)
			software_ddr3_reset(s);
	}

	/* After JEDEC reset */
	MCHBAR8_AND(0x40, ~0x2);
	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		reg32 = (2 << 18);
		reg32 |= post_jedec_tab[s->selected_timings.fsb_clk]
			[s->selected_timings.mem_clk - MEM_CLOCK_667MHz][0]
			<< 13;
		if (s->selected_timings.mem_clk == MEM_CLOCK_667MHz &&
			s->selected_timings.fsb_clk == FSB_CLOCK_1066MHz &&
			ch == 1) {
			reg32 |= (post_jedec_tab[s->selected_timings.fsb_clk]
				[s->selected_timings.mem_clk - MEM_CLOCK_667MHz][1]
				- 1) << 8;
		} else {
			reg32 |= post_jedec_tab[s->selected_timings.fsb_clk]
				[s->selected_timings.mem_clk - MEM_CLOCK_667MHz][1]
				<< 8;
		}
		MCHBAR32_AND_OR(0x400*ch + 0x274, ~0xfff00, reg32);
		MCHBAR8_AND(0x400*ch + 0x274, ~0x80);
		MCHBAR8_OR(0x400*ch + 0x26c, 1);
		MCHBAR32(0x400*ch + 0x278) = 0x88141881;
		MCHBAR16(0x400*ch + 0x27c) = 0x41;
		MCHBAR8(0x400*ch + 0x292) = 0xf2;
		MCHBAR8_OR(0x400*ch + 0x271, 0xe);
	}
	MCHBAR8_OR(0x2c4, 0x8);
	MCHBAR8_OR(0x2c3, 0x40);
	MCHBAR8_OR(0x2c4, 0x4);

	printk(BIOS_DEBUG, "Done post-jedec\n");

	/* Set DDR init complete */
	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		MCHBAR32_OR(0x400*ch + 0x268, 0xc0000000);
	}

	/* Dummy reads */
	if (s->boot_path == BOOT_PATH_NORMAL) {
		FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
			for (bank = 0; bank < 4; bank++)
				read32((u32 *)(test_address(ch, r) | 0x800000 | (bank << 12)));
		}
	}
	printk(BIOS_DEBUG, "Done dummy reads\n");

	/* Receive enable */
	sdram_program_receive_enable(s, fast_boot);
	printk(BIOS_DEBUG, "Done rcven\n");

	/* Finish rcven */
	FOR_EACH_CHANNEL(ch) {
		MCHBAR8_AND(0x400*ch + 0x5d8, ~0xe);
		MCHBAR8_OR(0x400*ch + 0x5d8, 0x2);
		MCHBAR8_OR(0x400*ch + 0x5d8, 0x4);
		MCHBAR8_OR(0x400*ch + 0x5d8, 0x8);
	}
	MCHBAR8_OR(0x5dc, 0x80);
	MCHBAR8_AND(0x5dc, ~0x80);
	MCHBAR8_OR(0x5dc, 0x80);

	/* XXX tRD */

	if (!fast_boot) {
		if (s->selected_timings.mem_clk > MEM_CLOCK_667MHz) {
			if (do_write_training(s))
				die("DQ write training failed!");
		}
		if (do_read_training(s))
			die("DQS read training failed!");
	}

	/* DRADRB */
	set_dradrb(s);
	printk(BIOS_DEBUG, "Done DRADRB\n");

	/* Memory map */
	configure_mmap(s);
	printk(BIOS_DEBUG, "Done memory map\n");

	/* Enhanced mode */
	set_enhanced_mode(s);
	printk(BIOS_DEBUG, "Done enhanced mode\n");

	/* Periodic RCOMP */
	MCHBAR16_AND_OR(0x160, ~0xfff, 0x999);
	MCHBAR16_OR(0x1b4, 0x3000);
	MCHBAR8_OR(0x130, 0x82);
	printk(BIOS_DEBUG, "Done PRCOMP\n");

	/* Power settings */
	power_settings(s);
	printk(BIOS_DEBUG, "Done power settings\n");

	/* ME related */
	/*
	 * FIXME: This locks some registers like bit1 of GGC
	 * and is only needed in case of ME being used.
	 */
	if (ME_UMA_SIZEMB != 0) {
		if (RANK_IS_POPULATED(s->dimms, 0, 0)
				|| RANK_IS_POPULATED(s->dimms, 1, 0))
			MCHBAR8_OR(0xa2f, 1 << 0);
		if (RANK_IS_POPULATED(s->dimms, 0, 1)
				|| RANK_IS_POPULATED(s->dimms, 1, 1))
			MCHBAR8_OR(0xa2f, 1 << 1);
		MCHBAR32_OR(0xa30, 1 << 26);
	}

	printk(BIOS_DEBUG, "Done raminit\n");
}
