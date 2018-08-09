/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
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

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <delay.h>
#include <halt.h>
#include <lib.h>
#include "pineview.h"
#include "raminit.h"
#include <pc80/mc146818rtc.h>
#include <spd.h>
#include <string.h>

/* Debugging macros. */
#if IS_ENABLED(CONFIG_DEBUG_RAM_SETUP)
#define PRINTK_DEBUG(x...)	printk(BIOS_DEBUG, x)
#else
#define PRINTK_DEBUG(x...)
#endif

#define MAX_TCLK_667	0x30
#define MAX_TCLK_800	0x25
#define MAX_TAC_667	0x45
#define MAX_TAC_800	0x40

#define NOP_CMD		(1 << 1)
#define PRE_CHARGE_CMD	(1 << 2)
#define MRS_CMD		((1 << 2) | (1 << 1))
#define EMRS_CMD	(1 << 3)
#define EMRS1_CMD	(EMRS_CMD | (1 << 4))
#define EMRS2_CMD	(EMRS_CMD | (1 << 5))
#define EMRS3_CMD	(EMRS_CMD | (1 << 5) | (1 << 4))
#define ZQCAL_CMD	((1 << 3) | (1 << 1))
#define CBR_CMD		((1 << 3) | (1 << 2))
#define NORMAL_OP_CMD	((1 << 3) | (1 << 2) | (1 << 1))

#define UBDIMM 1
#define SODIMM 2

#define TOTAL_CHANNELS 1
#define TOTAL_DIMMS 2

#define DIMM_IS_POPULATED(dimms, idx) (dimms[idx].card_type != 0)
#define IF_DIMM_POPULATED(dimms, idx) if (dimms[idx].card_type != 0)
#define ONLY_DIMMA_IS_POPULATED(dimms, ch) ( \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2) && \
	!DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3)))
#define ONLY_DIMMB_IS_POPULATED(dimms, ch) ( \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3) && \
	!DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2)))
#define BOTH_DIMMS_ARE_POPULATED(dimms, ch) ( \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2) && \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3))))
#define FOR_EACH_DIMM(idx) \
	for (idx = 0; idx < TOTAL_DIMMS; ++idx)
#define FOR_EACH_POPULATED_DIMM(dimms, idx) \
	FOR_EACH_DIMM(idx) IF_DIMM_POPULATED(dimms, idx)
#define CHANNEL_IS_POPULATED(dimms, idx) ((dimms[idx<<1].card_type != 0) || (dimms[(idx<<1) + 1].card_type != 0))
#define CHANNEL_IS_CARDF(dimms, idx) ((dimms[idx<<1].card_type == 0xf) || (dimms[(idx<<1) + 1].card_type == 0xf))
#define IF_CHANNEL_POPULATED(dimms, idx) if ((dimms[idx<<1].card_type != 0) || (dimms[(idx<<1) + 1].card_type != 0))
#define FOR_EACH_CHANNEL(idx) \
	for (idx = 0; idx < TOTAL_CHANNELS; ++idx)
#define FOR_EACH_POPULATED_CHANNEL(dimms, idx) \
	FOR_EACH_CHANNEL(idx) IF_CHANNEL_POPULATED(dimms, idx)

#define RANKS_PER_CHANNEL 4

#define FOR_EACH_RANK_IN_CHANNEL(r) \
	for (r = 0; r < RANKS_PER_CHANNEL; ++r)
#define FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r) \
	FOR_EACH_RANK_IN_CHANNEL(r) if (rank_is_populated(dimms, ch, r))
#define FOR_EACH_RANK(ch, r) \
	FOR_EACH_CHANNEL(ch) FOR_EACH_RANK_IN_CHANNEL(r)
#define FOR_EACH_POPULATED_RANK(dimms, ch, r) \
	FOR_EACH_RANK(ch, r) if (rank_is_populated(dimms, ch, r))

static bool rank_is_populated(struct dimminfo dimms[], u8 ch, u8 r)
{
	return ((dimms[ch<<1].card_type && ((r) < dimms[ch<<1].ranks))
		|| (dimms[(ch<<1) + 1].card_type
			&& ((r) >= 2)
			&& ((r) < (dimms[(ch<<1) + 1].ranks + 2))));
}

static inline void barrier(void)
{
	 __asm__ __volatile__("": : :"memory");
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

static int decode_spd(struct dimminfo *d, int i)
{
	d->type = 0;
	if (d->spd_data[20] == 0x2) {
		d->type = UBDIMM;
	} else if (d->spd_data[20] == 0x4) {
		d->type = SODIMM;
	}
	d->sides = (d->spd_data[5] & 0x7) + 1;
	d->banks = (d->spd_data[17] >> 2) - 1;
	d->chip_capacity = d->banks;
	d->rows = d->spd_data[3];// - 12;
	d->cols = d->spd_data[4];// - 9;
	d->cas_latencies = 0x78;
	d->cas_latencies &= d->spd_data[18];
	if (d->cas_latencies == 0)
		d->cas_latencies = 7;
	d->tAAmin = d->spd_data[26];
	d->tCKmin = d->spd_data[25];
	d->width = (d->spd_data[13] >> 3) - 1;
	d->page_size = (d->width+1) * (1 << d->cols); // Bytes
	d->tRAS = d->spd_data[30];
	d->tRP = d->spd_data[27];
	d->tRCD = d->spd_data[29];
	d->tWR = d->spd_data[36];
	d->ranks = d->sides; // XXX
#if IS_ENABLED(CONFIG_DEBUG_RAM_SETUP)
	const char *ubso[2] = { "UB", "SO" };
#endif
	PRINTK_DEBUG("%s-DIMM %d\n", &ubso[d->type][0], i);
	PRINTK_DEBUG("  Sides     : %d\n", d->sides);
	PRINTK_DEBUG("  Banks     : %d\n", d->banks);
	PRINTK_DEBUG("  Ranks     : %d\n", d->ranks);
	PRINTK_DEBUG("  Rows      : %d\n", d->rows);
	PRINTK_DEBUG("  Cols      : %d\n", d->cols);
	PRINTK_DEBUG("  Page size : %d\n", d->page_size);
	PRINTK_DEBUG("  Width     : %d\n", (d->width + 1) * 8);

	return 0;
}

/* Ram Config:    DIMMB-DIMMA
 *		0 EMPTY-EMPTY
 *		1 EMPTY-x16SS
 *		2 EMPTY-x16DS
 *		3 x16SS-x16SS
 *		4 x16DS-x16DS
 *		5 EMPTY- x8DS
 *		6 x8DS - x8DS
 */
static void find_ramconfig(struct sysinfo *s, u32 chan)
{
	if (s->dimms[chan>>1].sides == 0) {
		// NC
		if (s->dimms[(chan>>1) + 1].sides == 0) {
			// NC/NC
			s->dimm_config[chan] = 0;
		} else if (s->dimms[(chan>>1) + 1].sides == 1) {
			// NC/SS
			if (s->dimms[(chan>>1) + 1].width == 0) {
				// NC/8SS
				s->dimm_config[chan] = 1;
			} else {
				// NC/16SS
				s->dimm_config[chan] = 1;
			}
		} else {
			// NC/DS
			if (s->dimms[(chan>>1) + 1].width == 0) {
				// NC/8DS
				s->dimm_config[chan] = 5;
			} else {
				// NC/16DS
				s->dimm_config[chan] = 2;
			}
		}
	} else if (s->dimms[chan>>1].sides == 1) {
		// SS
		if (s->dimms[(chan>>1) + 1].sides == 0) {
			// SS/NC
			if (s->dimms[chan>>1].width == 0) {
				// 8SS/NC
				s->dimm_config[chan] = 1;
			} else {
				// 16SS/NC
				s->dimm_config[chan] = 1;
			}
		} else if (s->dimms[(chan>>1) + 1].sides == 1) {
			// SS/SS
			if (s->dimms[chan>>1].width == 0) {
				if (s->dimms[(chan>>1) + 1].width == 0) {
					// 8SS/8SS
					s->dimm_config[chan] = 3;
				} else {
					// 8SS/16SS
					die("Mixed Not supported\n");
				}
			} else {
				if (s->dimms[(chan>>1) + 1].width == 0) {
					// 16SS/8SS
					die("Mixed Not supported\n");
				} else {
					// 16SS/16SS
					s->dimm_config[chan] = 3;
				}
			}
		} else {
			// SS/DS
			if (s->dimms[chan>>1].width == 0) {
				if (s->dimms[(chan>>1) + 1].width == 0) {
					// 8SS/8DS
					die("Mixed Not supported\n");
				} else {
					die("Mixed Not supported\n");
				}
			} else {
				if (s->dimms[(chan>>1) + 1].width == 0) {
					// 16SS/8DS
					die("Mixed Not supported\n");
				} else {
					die("Mixed Not supported\n");
				}
			}
		}
	} else {
		// DS
		if (s->dimms[(chan>>1) + 1].sides == 0) {
			// DS/NC
			if (s->dimms[chan>>1].width == 0) {
				// 8DS/NC
				s->dimm_config[chan] = 5;
			} else {
				s->dimm_config[chan] = 4;
			}
		} else if (s->dimms[(chan>>1) + 1].sides == 1) {
			// DS/SS
			if (s->dimms[chan>>1].width == 0) {
				if (s->dimms[(chan>>1) + 1].width == 0) {
					// 8DS/8SS
					die("Mixed Not supported\n");
				} else {
					// 8DS/16SS
					die("Mixed Not supported\n");
				}
			} else {
				if (s->dimms[(chan>>1) + 1].width == 0) {
					die("Mixed Not supported\n");
				} else {
					// 16DS/16DS
					s->dimm_config[chan] = 4;
				}
			}
		} else {
			// DS/DS
			if (s->dimms[chan>>1].width == 0 && s->dimms[(chan>>1)+1].width == 0) {
				// 8DS/8DS
				s->dimm_config[chan] = 6;
			}
		}
	}
}

static void sdram_read_spds(struct sysinfo *s)
{
	u8 i, j, chan;
	int status = 0;
	s->dt0mode = 0;
	FOR_EACH_DIMM(i) {
		for (j = 0; j < 64; j++) {
			status = spd_read_byte(s->spd_map[i], j);
			if (status < 0) {
				s->dimms[i].card_type = 0;
				break;
			}
			s->dimms[i].spd_data[j] = (u8) status;
			if (j == 62)
				s->dimms[i].card_type = ((u8) status) & 0x1f;
		}
		hexdump(s->dimms[i].spd_data, 64);
	}

	s->spd_type = 0;
	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		switch (s->dimms[i].spd_data[2]) {
			case 0x8:
				s->spd_type = DDR2;
				break;
			case 0xb:
			default:
				die("DIMM type mismatch\n");
				break;
		}
	}

	int err = 1;
	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		err = decode_spd(&s->dimms[i], i);
		s->dt0mode |= (s->dimms[i].spd_data[49] & 0x2) >> 1;
	}
	if (err) {
		die("No memory dimms, halt\n");
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, chan) {
		find_ramconfig(s, chan);
		PRINTK_DEBUG("  Config[CH%d] : %d\n",
			chan, s->dimm_config[chan]);
	}
}

#if IS_ENABLED(CONFIG_DEBUG_RAM_SETUP)
static u32 fsb_reg_to_mhz(u32 speed)
{
	return (speed * 133) + 667;
}

static u32 ddr_reg_to_mhz(u32 speed)
{
	u32 mhz;
	mhz = (speed == 0) ? 667 :
		(speed == 1) ? 800 :
		0;
	return mhz;
}
#endif

static u8 lsbpos(u8 val) //Forward
{
	u8 i;
	for (i = 0; (i < 8) && ((val & (1 << i)) == 0); i++);
	return i;
}

static u8 msbpos(u8 val) //Reverse
{
	u8 i;
	for (i = 7; (i >= 0) && ((val & (1 << i)) == 0); i--);
	return i;
}

static void sdram_detect_smallest_params(struct sysinfo *s)
{
	static const u16 mult[6] = {
		3000, // 667
		2500, // 800
	};

	u8 i;
	u32 maxtras = 0;
	u32 maxtrp = 0;
	u32 maxtrcd = 0;
	u32 maxtwr = 0;
	u32 maxtrfc = 0;
	u32 maxtwtr = 0;
	u32 maxtrrd = 0;
	u32 maxtrtp = 0;

	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		maxtras = max(maxtras, s->dimms[i].spd_data[30] * 1000);
		maxtrp = max(maxtrp, (s->dimms[i].spd_data[27] * 1000) >> 2);
		maxtrcd = max(maxtrcd, (s->dimms[i].spd_data[29] * 1000) >> 2);
		maxtwr = max(maxtwr, (s->dimms[i].spd_data[36] * 1000) >> 2);
		maxtrfc = max(maxtrfc, s->dimms[i].spd_data[42] * 1000 +
				(s->dimms[i].spd_data[40] & 0xf));
		maxtwtr = max(maxtwtr, (s->dimms[i].spd_data[37] * 1000) >> 2);
		maxtrrd = max(maxtrrd, (s->dimms[i].spd_data[28] * 1000) >> 2);
		maxtrtp = max(maxtrtp, (s->dimms[i].spd_data[38] * 1000) >> 2);
	}
	/*
	 * TODO: on ddr3 there might be some minimal required values for some
	 * Timings: MIN_TRAS = 9, MIN_TRP = 3, MIN_TRCD = 3, MIN_TWR = 3,
	 * MIN_TWTR = 4, MIN_TRRD = 2, MIN_TRTP = 4
	 */
	s->selected_timings.tRAS = MIN(24, DIV_ROUND_UP(maxtras,
					mult[s->selected_timings.mem_clock]));
	s->selected_timings.tRP = MIN(10, DIV_ROUND_UP(maxtrp,
					mult[s->selected_timings.mem_clock]));
	s->selected_timings.tRCD = MIN(10, DIV_ROUND_UP(maxtrcd,
					mult[s->selected_timings.mem_clock]));
	s->selected_timings.tWR = MIN(15, DIV_ROUND_UP(maxtwr,
					mult[s->selected_timings.mem_clock]));
	/* Needs to be even */
	s->selected_timings.tRFC = 0xfe & (MIN(78, DIV_ROUND_UP(maxtrfc,
				mult[s->selected_timings.mem_clock])) + 1);
	s->selected_timings.tWTR = MIN(15, DIV_ROUND_UP(maxtwtr,
					mult[s->selected_timings.mem_clock]));
	s->selected_timings.tRRD = MIN(15, DIV_ROUND_UP(maxtrrd,
					mult[s->selected_timings.mem_clock]));
	s->selected_timings.tRTP = MIN(15, DIV_ROUND_UP(maxtrtp,
					mult[s->selected_timings.mem_clock]));

	PRINTK_DEBUG("Selected timings:\n");
	PRINTK_DEBUG("\tFSB:  %dMHz\n", fsb_reg_to_mhz(s->selected_timings.fsb_clock));
	PRINTK_DEBUG("\tDDR:  %dMHz\n", ddr_reg_to_mhz(s->selected_timings.mem_clock));

	PRINTK_DEBUG("\tCAS:  %d\n", s->selected_timings.CAS);
	PRINTK_DEBUG("\ttRAS: %d\n", s->selected_timings.tRAS);
	PRINTK_DEBUG("\ttRP:  %d\n", s->selected_timings.tRP);
	PRINTK_DEBUG("\ttRCD: %d\n", s->selected_timings.tRCD);
	PRINTK_DEBUG("\ttWR:  %d\n", s->selected_timings.tWR);
	PRINTK_DEBUG("\ttRFC: %d\n", s->selected_timings.tRFC);
	PRINTK_DEBUG("\ttWTR: %d\n", s->selected_timings.tWTR);
	PRINTK_DEBUG("\ttRRD: %d\n", s->selected_timings.tRRD);
	PRINTK_DEBUG("\ttRTP: %d\n", s->selected_timings.tRTP);
}

static void sdram_detect_ram_speed(struct sysinfo *s)
{
	u8 cas, reg8;
	u32 reg32;
	u32 freq = 0;
	u32 fsb = 0;
	u8 i;
	u8 commoncas = 0;
	u8 highcas = 0;
	u8 lowcas = 0;

	// Core frequency
	fsb = (pci_read_config8(PCI_DEV(0,0,0), 0xe3) & 0x70) >> 4;
	if (fsb) {
		fsb = 5 - fsb;
	} else {
		fsb = FSB_CLOCK_800MHz;
	}

	// DDR frequency
	freq = (pci_read_config8(PCI_DEV(0,0,0), 0xe3) & 0x80) >> 7;
	freq |= (pci_read_config8(PCI_DEV(0,0,0), 0xe4) & 0x3) << 1;
	if (freq) {
		freq = 6 - freq;
	} else {
		freq = MEM_CLOCK_800MHz;
	}

	// Detect a common CAS latency
	commoncas = 0xff;
	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		commoncas &= s->dimms[i].spd_data[18];
	}
	if (commoncas == 0) {
		die("No common CAS among dimms\n");
	}

	// Start with fastest common CAS
	cas = 0;
	highcas = msbpos(commoncas);
	lowcas = max(lsbpos(commoncas), 5);

	while (cas == 0 && highcas >= lowcas) {
		FOR_EACH_POPULATED_DIMM(s->dimms, i) {
			switch (freq) {
			case MEM_CLOCK_800MHz:
				if ((s->dimms[i].spd_data[9] > 0x25) ||
				    (s->dimms[i].spd_data[10] > 0x40)) {
					// CAS too fast, lower it
					highcas--;
					break;
				} else {
					cas = highcas;
				}
				break;
			case MEM_CLOCK_667MHz:
			default:
				if ((s->dimms[i].spd_data[9] > 0x30) ||
				    (s->dimms[i].spd_data[10] > 0x45)) {
					// CAS too fast, lower it
					highcas--;
					break;
				} else {
					cas = highcas;
				}
				break;
			}
		}
	}
	if (highcas < lowcas) {
		// Timings not supported by MCH, lower the frequency
		if (freq == MEM_CLOCK_800MHz) {
			freq--;
			PRINTK_DEBUG("Run DDR clock speed reduced due to timings\n");
		} else {
			die("Timings not supported by MCH\n");
		}
		cas = 0;
		highcas = msbpos(commoncas);
		lowcas = lsbpos(commoncas);
		while (cas == 0 && highcas >= lowcas) {
			FOR_EACH_POPULATED_DIMM(s->dimms, i) {
				switch (freq) {
				case MEM_CLOCK_800MHz:
					if ((s->dimms[i].spd_data[9] > 0x25) ||
					    (s->dimms[i].spd_data[10] > 0x40)) {
						// CAS too fast, lower it
						highcas--;
						break;
					} else {
						cas = highcas;
					}
					break;
				case MEM_CLOCK_667MHz:
				default:
					if ((s->dimms[i].spd_data[9] > 0x30) ||
					    (s->dimms[i].spd_data[10] > 0x45)) {
						// CAS too fast, lower it
						highcas--;
						break;
					} else {
						cas = highcas;
					}
					break;
				}
			}
		}
		if (cas == 0) {
			die("Unsupported dimms\n");
		}
	}

	s->selected_timings.CAS = cas;
	s->selected_timings.mem_clock = freq;
	s->selected_timings.fsb_clock = fsb;

	PRINTK_DEBUG("Drive Memory at %dMHz with CAS = %d clocks\n", ddr_reg_to_mhz(s->selected_timings.mem_clock), s->selected_timings.CAS);

	// Set memory frequency
	if (s->boot_path == BOOT_PATH_RESET)
		return;
	MCHBAR32(0xf14) = MCHBAR32(0xf14) | 0x1;
	reg32 = (MCHBAR32(0xc00) & (~0x70)) | (1 << 10);
	if (s->selected_timings.mem_clock == MEM_CLOCK_800MHz) {
		reg8 = 3;
	} else {
		reg8 = 2;
	}
	reg32 |= reg8 << 4;
	MCHBAR32(0xc00) = reg32;
	s->selected_timings.mem_clock = ((MCHBAR32(0xc00) >> 4) & 0x7) - 2;
	if (s->selected_timings.mem_clock == MEM_CLOCK_800MHz) {
		PRINTK_DEBUG("MCH validated at 800MHz\n");
		s->nodll = 0;
		s->maxpi = 63;
		s->pioffset = 0;
	} else if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		PRINTK_DEBUG("MCH validated at 667MHz\n");
		s->nodll = 1;
		s->maxpi = 15;
		s->pioffset = 1;
	} else {
		PRINTK_DEBUG("MCH set to unknown (%02x)\n",
			(uint8_t) s->selected_timings.mem_clock & 0xff);
	}
}

#define HPET_BASE 0xfed00000
#define HPET32(x) *((volatile u32 *)(HPET_BASE + x))
static void enable_hpet(void)
{
	u32 reg32;
	reg32 = RCBA32(0x3404);
	reg32 &= ~0x3;
	reg32 |= (1 << 7);
	RCBA32(0x3404) = reg32;
	HPET32(0x10) = HPET32(0x10) | 1;
}

static void sdram_clk_crossing(struct sysinfo *s)
{
	u8 clk_idx, fsb_idx;
	static const u32 clkcross[2][2][4] = {
	{
		{0xFFFFFFFF, 0x05030305, 0x0000FFFF, 0x00000000},  //667  667
		{0x1F1F1F1F, 0x2A1F1FA5, 0x00000000, 0x05000002},  //667  800
	},
	{
		{0x1F1F1F1F, 0x0D07070B, 0x00000000, 0x00000000},  //800  667
		{0xFFFFFFFF, 0x05030305, 0x0000FFFF, 0x00000000},  //800  800
	}
	};
	clk_idx = s->selected_timings.mem_clock;
	fsb_idx = s->selected_timings.fsb_clock;

	MCHBAR32(0xc04) = clkcross[fsb_idx][clk_idx][0];
	MCHBAR32(0xc50) = clkcross[fsb_idx][clk_idx][1];
	MCHBAR32(0xc54) = clkcross[fsb_idx][clk_idx][2];
	MCHBAR32(0xc28) = 0;
	MCHBAR32(0xc2c) = clkcross[fsb_idx][clk_idx][3];
	MCHBAR32(0xc08) = MCHBAR32(0xc08) | (1 << 7);

	if ((fsb_idx == 0) && (clk_idx == 1)) {
		MCHBAR8(0x6d4) = 0;
		MCHBAR32(0x700) = 0;
		MCHBAR32(0x704) = 0;
	}

	static const u32 clkcross2[2][2][8] = {
	{
		{  0,  0x08010204,  0,  0x08010204, 0, 0,  0,  0x04080102},  //  667  667
		{  0x04080000,  0x10010002,  0x10000000,  0x20010208,  0, 0x00000004,  0x02040000,  0x08100102},  //  667  800
	},
	{
		{  0x10000000,  0x20010208,  0x04080000,  0x10010002,  0, 0,  0x08000000,  0x10200204},  //  800  667
		{  0x00000000,  0x08010204,  0,  0x08010204,  0, 0,  0,  0x04080102},  //  800  800
	}
	};

	MCHBAR32(0x6d8) = clkcross2[fsb_idx][clk_idx][0];
	MCHBAR32(0x6e0) = clkcross2[fsb_idx][clk_idx][0];
	MCHBAR32(0x6e8) = clkcross2[fsb_idx][clk_idx][0];
	MCHBAR32(0x6d8+4) = clkcross2[fsb_idx][clk_idx][1];
	MCHBAR32(0x6e0+4) = clkcross2[fsb_idx][clk_idx][1];
	MCHBAR32(0x6e8+4) = clkcross2[fsb_idx][clk_idx][1];
	MCHBAR32(0x6f0) = clkcross2[fsb_idx][clk_idx][2];
	MCHBAR32(0x6f4) = clkcross2[fsb_idx][clk_idx][3];
	MCHBAR32(0x6f8) = clkcross2[fsb_idx][clk_idx][4];
	MCHBAR32(0x6fc) = clkcross2[fsb_idx][clk_idx][5];
	MCHBAR32(0x708) = clkcross2[fsb_idx][clk_idx][6];
	MCHBAR32(0x70c) = clkcross2[fsb_idx][clk_idx][7];
}

static void sdram_clkmode(struct sysinfo *s)
{
	u8 reg8;
	u16 reg16;

	MCHBAR16(0x1b6) = MCHBAR16(0x1b6) & ~(1 << 8);
	MCHBAR8(0x1b6) = MCHBAR8(0x1b6) & ~0x3f;

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		reg8 = 0;
		reg16 = 1;
	} else {
		reg8 = 1;
		reg16 = (1 << 8) | (1 << 5);
	}
	if (s->boot_path != BOOT_PATH_RESET)
		MCHBAR16(0x1c0) = (MCHBAR16(0x1c0) & ~(0x033f)) | reg16;

	MCHBAR32(0x220) = 0x58001117;
	MCHBAR32(0x248) = (MCHBAR32(0x248) | (1 << 23));

	const u32 cas_to_reg[2][4] = {
		{0x00000000, 0x00030100, 0x0C240201, 0x00000000}, // 667
		{0x00000000, 0x00030100, 0x0C240201, 0x10450302}  // 800
	};

	MCHBAR32(0x224) = cas_to_reg[reg8][s->selected_timings.CAS - 3];
}

static void sdram_timings(struct sysinfo *s)
{
	u8 i, j, ch, r, ta1, ta2, ta3, ta4, trp, bank, page, flag;
	u8 reg8, wl;
	u16 reg16;
	u32 reg32, reg2;
	static const u8 pagetab[2][2] = {{0xe, 0x12}, {0x10, 0x14}};

	// Only consider DDR2
	wl = s->selected_timings.CAS - 1;
	ta1 = ta2 = 6;
	ta3 = s->selected_timings.CAS;
	ta4 = 8;
	s->selected_timings.tRFC = (s->selected_timings.tRFC + 1) & 0xfe;
	trp = 0;
	bank = 1;
	page = 0;

	MCHBAR8(0x240) = ((wl - 3) << 4) | (s->selected_timings.CAS - 3);

	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		i = ch << 1;
		if (s->dimms[i].banks == 1) {
			trp = 1;
			bank = 0;
		}
		if (s->dimms[i].page_size == 2048) {
			page = 1;
		}
	}
	PRINTK_DEBUG("trp=%d bank=%d page=%d\n",trp, bank, page);

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		flag = 0;
	} else {
		flag = 1;
	}

	MCHBAR8(0x26f) = MCHBAR8(0x26f) | 0x3;
	MCHBAR16(0x250) = ((wl + 4 + s->selected_timings.tWR) << 6) |
				((2 + MAX(s->selected_timings.tRTP, 2)) << 2) | 1;
	reg32 = (bank << 21) | (s->selected_timings.tRRD << 17) |
		(s->selected_timings.tRP << 13) |
		((s->selected_timings.tRP + trp) << 9) |
		s->selected_timings.tRFC;
	if (bank == 0) {
		reg32 |= (pagetab[flag][page] << 22);
	}
	MCHBAR16(0x252) = (u16) reg32;
	MCHBAR16(0x254) = (u16) (reg32 >> 16);

	reg16 = (MCHBAR16(0x254) & 0xfc0) >> 6;
	MCHBAR16(0x62c) = (MCHBAR16(0x62c) & ~0x1f80) | (reg16 << 7);

	reg16 = (s->selected_timings.tRCD << 12) | (4 << 8) | (ta2 << 4) | ta4;
	MCHBAR16(0x256) = reg16;

	reg32 = (s->selected_timings.tRCD << 17) |
		((wl + 4 + s->selected_timings.tWTR) << 12) |
		(ta3 << 8) | (4 << 4) | ta1;
	MCHBAR32(0x258) = reg32;

	reg16 = ((s->selected_timings.tRP + trp) << 9) |
		s->selected_timings.tRFC;
	MCHBAR8(0x25b) = (u8) reg16;
	MCHBAR8(0x25c) = (u8) (reg16 >> 8);

	MCHBAR16(0x260) = (MCHBAR16(0x260) & ~0x3fe) | (100 << 1);
	MCHBAR8(0x25d) = (MCHBAR8(0x25d) & ~0x3f) | s->selected_timings.tRAS;
	MCHBAR16(0x244) = 0x2310;

	MCHBAR8(0x246) = (MCHBAR8(0x246) & ~0x1f) | 1;

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		reg32 = 3000;
	} else {
		reg32 = 2500;
	}
	if (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) {
		reg2 = 6000;
	} else {
		reg2 = 5000;
	}
	reg16 = (u16)((((s->selected_timings.CAS + 7)*(reg32)) / reg2) << 8);
	MCHBAR16(0x248) = (MCHBAR16(0x248) & ~0x1f00) | reg16;

	flag = 0;
	if (wl > 2) {
		flag = 1;
	}
	reg16 = (u8) (wl - 1 - flag);
	reg16 |= reg16 << 4;
	reg16 |= flag << 8;
	MCHBAR16(0x24d) = (MCHBAR16(0x24d) & ~0x1ff) | reg16;

	MCHBAR16(0x25e) = 0x1585;
	MCHBAR8(0x265) = MCHBAR8(0x265) & ~0x1f;
	MCHBAR16(0x265) = (MCHBAR16(0x265) & ~0x3f00) |
		((s->selected_timings.CAS + 9) << 8);

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		reg16 = 0x514;
		reg32 = 0xa28;
	} else {
		reg16 = 0x618;
		reg32 = 0xc30;
	}
	MCHBAR32(0x268) = (MCHBAR32(0x268) & ~0xfffff00) |
		(0x3f << 22) | (reg32 << 8);
	MCHBAR8(0x26c) = 0x00;
	MCHBAR16(0x2b8) = (MCHBAR16(0x2b8) & 0xc000) | reg16;
	MCHBAR8(0x274) = MCHBAR8(0x274) | 1;

	MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0x7f000000) | (0xb << 25);
	i = s->selected_timings.mem_clock;
	j = s->selected_timings.fsb_clock;
	if (i > j) {
		MCHBAR32(0x248) = MCHBAR32(0x248) | (1 << 24);
	}

	MCHBAR8(0x24c) = MCHBAR8(0x24c) & ~0x3;
	MCHBAR16(0x24d) = (MCHBAR16(0x24d) & ~0x7c00) | ((wl + 10) << 10);
	MCHBAR32(0x260) = (MCHBAR32(0x260) & ~0x70e0000) | (3 << 24) | (3 << 17);
	reg16 = 0x15 << 6;
	reg16 |= 0x1f;
	reg16 |= (0x6 << 12);
	MCHBAR16(0x26d) = (MCHBAR16(0x26d) & ~0x7fff) | reg16;

	reg32 = (0x6 << 27) | (1 << 25);
	MCHBAR32(0x268) = (MCHBAR32(0x268) & ~0x30000000) | ((u32)(reg32 << 8));
	MCHBAR8(0x26c) = (MCHBAR8(0x26c) & ~0xfa) | ((u8)(reg32 >> 24));
	MCHBAR8(0x271) = MCHBAR8(0x271) & ~(1 << 7);
	MCHBAR8(0x274) = MCHBAR8(0x274) & ~0x6;
	reg32 = (u32) (((6 & 0x03) << 30) | (4 << 25) | (1 << 20) | (8 << 15) |
			(6 << 10) | (4 << 5) | 1);
	MCHBAR32(0x278) = reg32;

	MCHBAR16(0x27c) = (MCHBAR16(0x27c) & ~0x1ff) | (8 << 3) | (6 >> 2);
	MCHBAR16(0x125) = MCHBAR16(0x125) | 0x1c00 | (0x1f << 5);
	MCHBAR8(0x127) = (MCHBAR8(0x127) & ~0xff) | 0x40;
	MCHBAR8(0x128) = (MCHBAR8(0x128) & ~0x7) | 0x5;
	MCHBAR8(0x129) = MCHBAR8(0x129) | 0x1f;
	reg8 = 3 << 6;
	reg8 |= (s->dt0mode << 4);
	reg8 |= 0x0c;
	MCHBAR8(0x12f) = (MCHBAR8(0x12f) & ~0xdf) | reg8;
	MCHBAR8(0x18c) = MCHBAR8(0x18c) & ~0x2;
	MCHBAR8(0x228) = (MCHBAR8(0x228) & ~0x7) | 0x2;
	MCHBAR16(0x241) = (MCHBAR16(0x241) & ~0x3fc) | (4 << 2);
	reg32 = (2 << 29) | (1 << 28) | (1 << 23);
	MCHBAR32(0x120) = (MCHBAR32(0x120) & ~0xffb00000) | reg32;

	reg8 = (u8) ((MCHBAR16(0x252) & 0xe000) >> 13);
	reg8 |= (u8) ((MCHBAR16(0x254) & 1) << 3);
	MCHBAR8(0x12d) = (MCHBAR8(0x12d) & ~0xf0) | (reg8 << 4);
	reg8 = (u8) ((MCHBAR32(0x258) & 0xf0000) >> 17);
	MCHBAR8(0x12d) = (MCHBAR8(0x12d) & ~0xf) | reg8;
	MCHBAR8(0x12e) = MCHBAR8(0x12e) & ~0xfc;
	MCHBAR8(0x12e) = MCHBAR8(0x12e) & ~0x3;
	MCHBAR8(0x12f) = MCHBAR8(0x12f) & ~0x3;
	MCHBAR8(0x241) = MCHBAR8(0x241) | 1;
	MCHBAR16(0x1b6) = MCHBAR16(0x1b6) | (1 << 9);
	for (i = 0; i < 8; i++) {
		MCHBAR32(0x540 + i*4) = (MCHBAR32(0x540 + i*4) & ~0x3f3f3f3f) |
			0x0c0c0c0c;
	}
	MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0xf0000) |
		((s->selected_timings.CAS + 1) << 16);
	for (i = 0; i < 8; i++) {
		MCHBAR8(0x560 + i*4) = MCHBAR8(0x560 + i*4) & ~0x3f;
		MCHBAR16(0x58c) = MCHBAR16(0x58c) & ((u16) (~(3 << (i*2))));
		MCHBAR16(0x588) = MCHBAR16(0x588) & ((u16) (~(3 << (i*2))));
		MCHBAR16(0x5fa) = MCHBAR16(0x5fa) & ((u16) (~(3 << (i*2))));
	}
	MCHBAR8(0x5f0) = MCHBAR8(0x5f0) & ~0x1;
	MCHBAR8(0x5f0) = MCHBAR8(0x5f0) | 0x2;
	MCHBAR8(0x5f0) = MCHBAR8(0x5f0) | 0x4;
	MCHBAR32(0x2c0) = MCHBAR32(0x2c0) | 0xc0400;
	MCHBAR32(0x594) = MCHBAR32(0x594) | (1 << 31);
}

static void sdram_p_clkset0(const struct pllparam *pll, u8 f, u8 i)
{
	MCHBAR16(0x5a0) = (MCHBAR16(0x5a0) & ~0xc440) |
			(pll->clkdelay[f][i] << 14) |
			(pll->dben[f][i] << 10) |
			(pll->dbsel[f][i] << 6);
	MCHBAR8(0x581) = (MCHBAR8(0x581) & ~0x3f) | pll->pi[f][i];
}

static void sdram_p_clkset1(const struct pllparam *pll, u8 f, u8 i)
{
	MCHBAR16(0x5a0) = (MCHBAR16(0x5a0) & ~0x30880) |
			(pll->clkdelay[f][i] << 16) |
			(pll->dben[f][i] << 11) |
			(pll->dbsel[f][i] << 7);
	MCHBAR8(0x582) = (MCHBAR8(0x582) & ~0x3f) | pll->pi[f][i];
}

static void sdram_p_cmd(const struct pllparam *pll, u8 f, u8 i)
{
	u8 reg8;
	reg8 = pll->dbsel[f][i] << 5;
	reg8 |= pll->dben[f][i] << 6;
	MCHBAR8(0x594) = (MCHBAR8(0x594) & ~0x60) | reg8;

	reg8 = pll->clkdelay[f][i] << 4;
	MCHBAR8(0x598) = (MCHBAR8(0x598) & ~0x30) | reg8;

	reg8 = pll->pi[f][i];
	MCHBAR8(0x580) = (MCHBAR8(0x580) & ~0x3f) | reg8;
	MCHBAR8(0x583) = (MCHBAR8(0x583) & ~0x3f) | reg8;
}

static void sdram_p_ctrl(const struct pllparam *pll, u8 f, u8 i)
{
	u8 reg8;
	u32 reg32;
	reg32 = ((u32) pll->dbsel[f][i]) << 20;
	reg32 |= ((u32) pll->dben[f][i]) << 21;
	reg32 |= ((u32) pll->dbsel[f][i]) << 22;
	reg32 |= ((u32) pll->dben[f][i]) << 23;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 24;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 27;
	MCHBAR32(0x59c) = (MCHBAR32(0x59c) & ~0x1bf0000) | reg32;

	reg8 = pll->pi[f][i];
	MCHBAR8(0x584) = (MCHBAR8(0x584) & ~0x3f) | reg8;
	MCHBAR8(0x585) = (MCHBAR8(0x585) & ~0x3f) | reg8;

	reg32 = ((u32) pll->dbsel[f][i]) << 12;
	reg32 |= ((u32) pll->dben[f][i]) << 13;
	reg32 |= ((u32) pll->dbsel[f][i]) << 8;
	reg32 |= ((u32) pll->dben[f][i]) << 9;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 14;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 10;
	MCHBAR32(0x598) = (MCHBAR32(0x598) & ~0xff00) | reg32;

	reg8 = pll->pi[f][i];
	MCHBAR8(0x586) = (MCHBAR8(0x586) & ~0x3f) | reg8;
	MCHBAR8(0x587) = (MCHBAR8(0x587) & ~0x3f) | reg8;
}

static void sdram_p_dqs(struct pllparam *pll, u8 f, u8 clk)
{
	u8 rank, dqs, reg8, j;
	u32 reg32;

	j = clk - 40;
	reg8 = 0;
	reg32 = 0;
	rank = j % 4;
	dqs = j / 4;

	reg32 |= ((u32) pll->dben[f][clk]) << (dqs + 9);
	reg32 |= ((u32) pll->dbsel[f][clk]) << dqs;
	MCHBAR32(0x5b4+rank*4) = (MCHBAR32(0x5b4+rank*4) &
		~( (1 << (dqs+9))|(1 << dqs) )) | reg32;

	reg32 = ((u32) pll->clkdelay[f][clk]) << ((dqs*2) + 16);
	MCHBAR32(0x5c8+rank*4) = (MCHBAR32(0x5c8+rank*4) &
		~( (1 << (dqs*2 + 17))|(1 << (dqs*2 + 16)) )) | reg32;

	reg8 = pll->pi[f][clk];
	MCHBAR8(0x520+j) = (MCHBAR8(0x520+j) & ~0x3f) | reg8;
}


static void sdram_p_dq(struct pllparam *pll, u8 f, u8 clk)
{
	u8 rank, dq, reg8, j;
	u32 reg32;

	j = clk - 8;
	reg8 = 0;
	reg32 = 0;
	rank = j % 4;
	dq = j / 4;

	reg32 |= ((u32) pll->dben[f][clk]) << (dq + 9);
	reg32 |= ((u32) pll->dbsel[f][clk]) << dq;
	MCHBAR32(0x5a4+rank*4) = (MCHBAR32(0x5a4+rank*4) &
		~( (1 << (dq+9))|(1 << dq) )) | reg32;

	reg32 = ((u32) pll->clkdelay[f][clk]) << (dq*2);
	MCHBAR32(0x5c8+rank*4) = (MCHBAR32(0x5c8+rank*4) &
		~( (1 << (dq*2 + 1))|(1 << (dq*2)) )) | reg32;

	reg8 = pll->pi[f][clk];
	MCHBAR8(0x500+j) = (MCHBAR8(0x500+j) & ~0x3f) | reg8;
}

static void sdram_calibratepll(struct sysinfo *s, u8 pidelay)
{
	struct pllparam pll = {
		.pi = {
		{	// 667
			3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4,
			4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
			7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 3, 3
		},
		{	// 800
			53, 53, 10, 10, 5,  5,  5,  5,  27, 27, 27, 27,
			34, 34, 34, 34, 34, 34, 34, 34, 39, 39, 39, 39,
			47, 47, 47, 47, 44, 44, 44, 44, 47, 47, 47, 47,
			47, 47, 47, 47, 59, 59, 59, 59, 2,  2,  2,  2,
			2,  2,  2,  2,  7,  7,  7,  7,  15, 15, 15, 15,
			12, 12, 12, 12, 15, 15, 15, 15, 15, 15, 15, 15
		}},

		.dben = {
		{	// 667
			0,0,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0
		},
		{	// 800
			1,1,1,1,1,1,1,1,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,0,0,0,0,
			1,1,1,1,0,0,0,0,0,0,0,0
		}},

		.dbsel = {
		{	// 667
			0,0,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0
		},
		{	// 800
			0,0,1,1,1,1,1,1,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,1,1,1,1,
			1,1,1,1,1,1,1,1,0,0,0,0,
			1,1,1,1,0,0,0,0,0,0,0,0
		}},

		.clkdelay = {
		{	// 667
			0,0,1,1,0,0,0,0,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0
		},
		{	// 800
			0,0,0,0,0,0,0,0,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,0,0,0,0,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1
		}}
	};

	u8 i, f;
	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		f = 0;
	} else {
		f = 1;
	}
	for (i = 0; i < 72; i++) {
		pll.pi[f][i] += pidelay;
	}

	MCHBAR8(0x1a4) = MCHBAR8(0x1a4) & ~(1 << 7);
	MCHBAR16(0x190) = (MCHBAR16(0x190) & (u16) ~(0x3fff)) | 0x1fff;

	sdram_p_clkset0(&pll, f, 0);
	sdram_p_clkset1(&pll, f, 1);
	sdram_p_cmd(&pll, f, 2);
	sdram_p_ctrl(&pll, f, 4);
	for (i = 0; i < 32; i++) {
		sdram_p_dqs(&pll, f, i+40);
	}
	for (i = 0; i < 32; i++) {
		sdram_p_dq(&pll, f, i+8);
	}
}

static void sdram_calibratehwpll(struct sysinfo *s)
{
	u8 reg8;

	s->async = 0;
	reg8 = 0;
	MCHBAR16(0x180) = MCHBAR16(0x180) | (1 << 15);
	MCHBAR8(0x180) = MCHBAR8(0x180) & ~(1 << 7);
	MCHBAR8(0x180) = MCHBAR8(0x180) | (1 << 3);
	MCHBAR8(0x180) = MCHBAR8(0x180) | (1 << 2);

	MCHBAR8(0x180) = MCHBAR8(0x180) | (1 << 7);
	while ((MCHBAR8(0x180) & (1 << 2)) == 0);

	reg8 = (MCHBAR8(0x180) & (1 << 3)) >> 3;
	if (reg8 != 0) {
		s->async = 1;
	}
}

static void sdram_dlltiming(struct sysinfo *s)
{
	u8 reg8, i, pipost;
	u16 reg16;
	u32 reg32;

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		reg32 = 0x8014227;
	} else {
		reg32 = 0x14221;
	}
	MCHBAR32(0x19c) = (MCHBAR32(0x19c) & ~0xfffffff) | reg32;
	MCHBAR32(0x19c) = MCHBAR32(0x19c) | (1 << 23);
	MCHBAR32(0x19c) = MCHBAR32(0x19c) | (1 << 15);
	MCHBAR32(0x19c) = MCHBAR32(0x19c) & ~(1 << 15);

	if (s->nodll) {
		MCHBAR16(0x198) = MCHBAR16(0x198) | (1 << 0);
		MCHBAR16(0x198) = MCHBAR16(0x198) | (1 << 2);
		MCHBAR16(0x198) = MCHBAR16(0x198) | (1 << 4);
		MCHBAR16(0x198) = MCHBAR16(0x198) | (1 << 8);
		MCHBAR16(0x198) = MCHBAR16(0x198) | (1 << 10);
		MCHBAR16(0x198) = MCHBAR16(0x198) | (1 << 12);
		MCHBAR16(0x198) = MCHBAR16(0x198) | (1 << 14);
	} else {
		MCHBAR16(0x198) = MCHBAR16(0x198) & ~(1 << 0);
		MCHBAR16(0x198) = MCHBAR16(0x198) & ~(1 << 2);
		MCHBAR16(0x198) = MCHBAR16(0x198) & ~(1 << 4);
		MCHBAR16(0x198) = MCHBAR16(0x198) & ~(1 << 8);
		MCHBAR16(0x198) = MCHBAR16(0x198) & ~(1 << 10);
		MCHBAR16(0x198) = MCHBAR16(0x198) & ~(1 << 12);
		MCHBAR16(0x198) = MCHBAR16(0x198) & ~(1 << 14);
	}

	if (s->nodll) {
		MCHBAR8(0x1c8) = (MCHBAR8(0x1c8) & ~0x3f) | 0x7;
	} else {
		MCHBAR8(0x1c8) = (MCHBAR8(0x1c8) & ~0x3f);
	}

	sdram_calibratepll(s, 0); // XXX check

	MCHBAR16(0x5f0) = MCHBAR16(0x5f0) | (1 << 11);
	MCHBAR16(0x5f0) = MCHBAR16(0x5f0) | (1 << 12);

	for (i = 0; i < 8; i++) {
		MCHBAR16(0x5f0) = MCHBAR16(0x5f0) | ((1 << 10) >> i);
	}
	MCHBAR8(0x2c14) = MCHBAR8(0x2c14) | 1;
	MCHBAR16(0x182) = 0x5005;
	MCHBAR16(0x18a) = (MCHBAR16(0x18a) & ~0x1f1f) | 0x51a;
	MCHBAR16(0x2c00) = (MCHBAR16(0x2c00) & ~0xbf3f) | 0x9010;

	if (s->nodll) {
		MCHBAR8(0x18e) = (MCHBAR8(0x18e) & ~0x7f) | 0x6b;
	} else {
		MCHBAR8(0x18e) = (MCHBAR8(0x18e) & ~0x7f) | 0x55;
		sdram_calibratehwpll(s);
	}
	pipost = 0x34;

	MCHBAR32(0x248) = MCHBAR32(0x248) & ~(1 << 22);
	MCHBAR8(0x5d9) = MCHBAR8(0x5d9) & ~0x2;
	MCHBAR8(0x189) = MCHBAR8(0x189) | 0xc0;
	MCHBAR8(0x189) = MCHBAR8(0x189) & ~(1 << 5);
	MCHBAR8(0x189) = (MCHBAR8(0x189) & ~0xc0) | (1 << 6);
	MCHBAR8(0x188) = (MCHBAR8(0x188) & ~0x3f) | 0x1a;
	MCHBAR8(0x188) = MCHBAR8(0x188) | 1;

	MCHBAR8(0x1a8) = MCHBAR8(0x1a8) | 1;
	MCHBAR32(0x1a0) = 0x551803;
	if (ONLY_DIMMA_IS_POPULATED(s->dimms, 0)) {
		reg8 = 0x3c;
	} else if (ONLY_DIMMB_IS_POPULATED(s->dimms, 0)) {
		reg8 = 0x27;
	} else if (BOTH_DIMMS_ARE_POPULATED(s->dimms, 0)) {
		reg8 = 0x24;
	} else {
		// None
		reg8 = 0x3f;
	}
	reg8 = 0x00; //switch all clocks on anyway

	MCHBAR32(0x5a0) = (MCHBAR32(0x5a0) & ~0x3f000000) | (reg8 << 24);
	MCHBAR8(0x594) = MCHBAR8(0x594) & ~1;
	reg16 = 0;
	if (!rank_is_populated(s->dimms, 0, 0)) {
		reg16 |= (1 << 8) | (1 << 4) | (1 << 0);
	}
	if (!rank_is_populated(s->dimms, 0, 1)) {
		reg16 |= (1 << 9) | (1 << 5) | (1 << 1);
	}
	if (!rank_is_populated(s->dimms, 0, 2)) {
		reg16 |= (1 << 10) | (1 << 6) | (1 << 2);
	}
	if (!rank_is_populated(s->dimms, 0, 3)) {
		reg16 |= (1 << 11) | (1 << 7) | (1 << 3);
	}
	MCHBAR16(0x59c) = MCHBAR16(0x59c) | reg16;
}

static void sdram_rcomp(struct sysinfo *s)
{
	u8 i, j, reg8, f, rcompp, rcompn, srup, srun;
	u16 reg16;
	u32 reg32, rcomp1, rcomp2;

	static const u8 rcompupdate[7] = { 0, 0, 0, 1, 1, 0, 0 };
	static const u8 rcompslew = 0xa;
	static const u8 rcompstr[7] = { 0x66, 0, 0xaa, 0x55, 0x55, 0x77, 0x77 };
	static const u16 rcompscomp[7] = { 0xa22a, 0, 0xe22e, 0xe22e, 0xe22e, 0xa22a, 0xa22a };
	static const u8 rcompdelay[7] = { 1, 0, 0, 0, 0, 1, 1 };
	static const u16 rcompctl[7] = { 0x31c, 0, 0x374, 0x3a2, 0x3d0, 0x3fe, 0x42c };
	static const u16 rcompf[7] = { 0x1114, 0, 0x0505, 0x0909, 0x0909, 0x0a0a, 0x0a0a };

	//                   NC-NC   x16SS   x16DS  x16SS2  x16DS2 x8DS, x8DS2
	static const u8 rcompstr2[7]    = {   0x00,   0x55,   0x55,   0xaa,
					      0xaa , 0x55,   0xaa};
	static const u16 rcompscomp2[7] = { 0x0000, 0xe22e, 0xe22e, 0xe22e,
					    0x8228 , 0xe22e, 0x8228 };
	static const u8 rcompdelay2[7]  = {      0,      0,      0,      0,      2 , 0,      2};

	static const u8 rcomplut[64][12] = {
		{ 9, 9,11,11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 9, 9,11, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 10,9,12, 11, 2, 2, 5,5, 6, 6,5, 5},
		{ 10,9,12, 11, 2, 2, 6,5, 7, 6,6, 5},
		{ 10,10,12, 12, 2, 2, 6,5, 7, 6,6, 5},
		{ 10,10,12, 12, 2, 2, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 2, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 2, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 2, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 3, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 3, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 3, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 3, 6,6, 7, 7,6, 6},
		{ 10,10,12, 12, 3, 3, 6,6, 7, 7,6, 6},
		{ 11,10,12, 12, 3, 3, 6,6, 7, 7,6, 6},
		{ 11,10,14, 13, 3, 3, 6,6, 7, 7,6, 6},
		{ 12,10,14, 13, 3, 3, 6,6, 7, 7,6, 6},
		{ 12,12,14, 13, 3, 3, 7,6, 7, 7,7, 6},
		{ 13,12,16, 15, 3, 3, 7,6, 8, 7,7, 6},
		{ 13,14,16, 15, 4, 3, 7,7, 8, 8,7, 7},
		{ 14,14,16, 17, 4, 3, 7,7, 8, 8,7, 7},
		{ 14,16,18, 17, 4, 4, 8,7, 8, 8,8, 7},
		{ 15,16,18, 19, 4, 4, 8,7, 9, 8,8, 7},
		{ 15,18,18, 19, 4, 4, 8,8, 9, 9,8, 8},
		{ 16,18,20, 21, 4, 4, 8,8, 9, 9,8, 8},
		{ 16,19,20, 21, 5, 4, 9,8, 10, 9,9, 8},
		{ 16,19,20, 23, 5, 5, 9,9, 10, 10,9, 9},
		{ 17,19,22, 23, 5, 5, 9,9, 10, 10,9, 9},
		{ 17,20,22, 25, 5, 5, 9,9, 10, 10,9, 9},
		{ 17,20,22, 25, 5, 5, 9,9, 10, 10,9, 9},
		{ 18,20,22, 25, 5, 5, 9,9, 10, 10,9, 9},
		{ 18,21,24, 25, 5, 5, 9,9, 11, 10,9, 9},
		{ 19,21,24, 27, 5, 5, 9, 9, 11, 11,9, 9},
		{ 19,22,24, 27, 5, 5, 10,9, 11, 11,10, 9},
		{ 20,22,24, 27, 6, 5, 10,10, 11, 11,10, 10},
		{ 20,23,26, 27, 6, 6, 10,10, 12, 12,10, 10},
		{ 20,23,26, 29, 6, 6, 10,10, 12, 12,10, 10},
		{ 21,24,26, 29, 6, 6, 10,10, 12, 12,10, 10},
		{ 21,24,26, 29, 6, 6, 11,10, 12, 13,11, 10},
		{ 22,25,28, 29, 6, 6, 11,11, 13, 13,11, 11},
		{ 22,25,28, 31, 6, 6, 11,11, 13, 13,11, 11},
		{ 22,26,28, 31, 6, 6, 11,11, 13, 14,11, 11},
		{ 23,26,30, 31, 7, 6, 12,11, 14, 14,12, 11},
		{ 23,27,30, 33, 7, 7, 12,12, 14, 14,12, 12},
		{ 23,27,30, 33, 7, 7, 12,12, 14, 15,12, 12},
		{ 24,28,32, 33, 7, 7, 12,12, 15, 15,12, 12},
		{ 24,28,32, 33, 7, 7, 12,12, 15, 16,12, 12},
		{ 24,29,32, 35, 7, 7, 12,12, 15, 16,12, 12},
		{ 25,29,32, 35, 7, 7, 12,12, 15, 17,12, 12},
		{ 25,30,32, 35, 7, 7, 12,12, 15, 17,12, 12},
		{ 25,30,32, 35, 7, 7, 12,12, 15, 17,12, 12},
	};

	srup = 0;
	srun = 0;

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		f = 0;
		rcomp1 = 0x00050431;
	} else {
		f = 1;
		rcomp1 = 0x00050542;
	}
	if (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) {
		rcomp2 = 0x14C42827;
	} else {
		rcomp2 = 0x19042827;
	}

	for (i = 0; i < 7; i++) {
		if (i == 1)
			continue;
		reg8 = rcompupdate[i];
		MCHBAR8(rcompctl[i]) = (MCHBAR8(rcompctl[i]) & ~0x1) | reg8;
		MCHBAR8(rcompctl[i]) = MCHBAR8(rcompctl[i]) & ~0x2;
		reg16 = (u16) rcompslew;
		MCHBAR16(rcompctl[i]) = (MCHBAR16(rcompctl[i]) & ~0xf000) |
				(reg16 << 12);
		MCHBAR8(rcompctl[i]+4) = rcompstr[i];
		MCHBAR16(rcompctl[i]+0xe) = rcompscomp[i];
		MCHBAR8(rcompctl[i]+0x14) = (MCHBAR8(rcompctl[i]+0x14) & ~0x3) |
				rcompdelay[i];
		if (i == 2) {
			reg16 = (u16) rcompslew;
			MCHBAR16(rcompctl[i]) = (MCHBAR16(rcompctl[i]) &
					~0xf000) | (reg16 << 12);
			MCHBAR8(rcompctl[i]+4) = rcompstr2[s->dimm_config[0]];
			MCHBAR16(rcompctl[i]+0xe) = rcompscomp2[s->dimm_config[0]];
			MCHBAR8(rcompctl[i]+0x14) = (MCHBAR8(rcompctl[i]+0x14) &
					~0x3) | rcompdelay2[s->dimm_config[0]];
		}

		MCHBAR16(rcompctl[i]+0x16) = MCHBAR16(rcompctl[i]+0x16) & ~0x7f7f;
		MCHBAR16(rcompctl[i]+0x18) = MCHBAR16(rcompctl[i]+0x18) & ~0x3f3f;
		MCHBAR16(rcompctl[i]+0x1a) = MCHBAR16(rcompctl[i]+0x1a) & ~0x3f3f;
		MCHBAR16(rcompctl[i]+0x1c) = MCHBAR16(rcompctl[i]+0x1c) & ~0x3f3f;
		MCHBAR16(rcompctl[i]+0x1e) = MCHBAR16(rcompctl[i]+0x1e) & ~0x3f3f;
	}

	MCHBAR8(0x45a) = (MCHBAR8(0x45a) & ~0x3f) | 0x36;
	MCHBAR8(0x462) = (MCHBAR8(0x462) & ~0x3f) | 0x36;

	for (i = 0; i < 7; i++) {
		if (i == 1)
			continue;
		MCHBAR8(rcompctl[i]) = MCHBAR8(rcompctl[i]) & ~0x60;
		MCHBAR16(rcompctl[i]+2) = MCHBAR16(rcompctl[i]+2) & ~0x706;
		MCHBAR16(rcompctl[i]+0xa) = MCHBAR16(rcompctl[i]+0xa) & ~0x7f7f;
		MCHBAR16(rcompctl[i]+0x12) = MCHBAR16(rcompctl[i]+0x12) & ~0x3f3f;
		MCHBAR16(rcompctl[i]+0x24) = MCHBAR16(rcompctl[i]+0x24) & ~0x1f1f;
		MCHBAR8(rcompctl[i]+0x26) = MCHBAR8(rcompctl[i]+0x26) & ~0x1f;
	}

	MCHBAR16(0x45a) = MCHBAR16(0x45a) & ~0xffc0;
	MCHBAR16(0x45c) = MCHBAR16(0x45c) & ~0xf;
	MCHBAR16(0x462) = MCHBAR16(0x462) & ~0xffc0;
	MCHBAR16(0x464) = MCHBAR16(0x464) & ~0xf;

	for (i = 0; i < 7; i++) {
		if (i == 1)
			continue;
		MCHBAR16(rcompctl[i]+0x10) = rcompf[i];
		MCHBAR16(rcompctl[i]+0x20) = 0x1219;
		MCHBAR16(rcompctl[i]+0x22) = 0x000C;
	}

	MCHBAR32(0x164) = (MCHBAR32(0x164) & ~0x1f1f1f) | 0x0c1219;
	MCHBAR16(0x4b0) = (MCHBAR16(0x4b0) & ~0x1f00) | 0x1200;
	MCHBAR8(0x4b0) = (MCHBAR8(0x4b0) & ~0x1f) | 0x12;
	MCHBAR32(0x138) = 0x007C9007;
	MCHBAR32(0x16c) = rcomp1;
	MCHBAR16(0x17a) = 0x1f7f;
	MCHBAR32(0x134) = rcomp2;
	MCHBAR16(0x170) = (MCHBAR16(0x170) & ~0xf) | 1;
	MCHBAR16(0x178) = 0x134;
	MCHBAR32(0x130) = 0x4C293600;
	MCHBAR8(0x133) = (MCHBAR8(0x133) & ~0x44) | (1 << 6) | (1 << 2);
	MCHBAR16(0x4b0) = MCHBAR16(0x4b0) & ~(1 << 13);
	MCHBAR8(0x4b0) = MCHBAR8(0x4b0) & ~(1 << 5);

	for (i = 0; i < 7; i++) {
		if (i == 1)
			continue;
		MCHBAR8(rcompctl[i]+2) = MCHBAR8(rcompctl[i]) & ~0x71;
	}

	if ((MCHBAR32(0x130) & (1 << 30)) == 0) {
		MCHBAR8(0x130) = MCHBAR8(0x130) | 0x1;
		while ((MCHBAR8(0x130) & 0x1) != 0);

		reg32 = MCHBAR32(0x13c);
		rcompp = (u8) ((reg32 & ~(1 << 31)) >> 24);
		rcompn = (u8) ((reg32 & ~(0xff800000)) >> 16);

		for (i = 0; i < 7; i++) {
			if (i == 1)
				continue;
			srup = (MCHBAR8(rcompctl[i]+1) & 0xc0) >> 6;
			srun = (MCHBAR8(rcompctl[i]+1) & 0x30) >> 4;
			reg16 = (u16)(rcompp - (1 << (srup + 1))) << 8;
			MCHBAR16(rcompctl[i]+0x16) = (MCHBAR16(rcompctl[i]+0x16)
							& ~0x7f00) | reg16;
			reg16 = (u16)(rcompn - (1 << (srun + 1)));
			MCHBAR8(rcompctl[i]+0x16) = (MCHBAR8(rcompctl[i]+0x16) &
							~0x7f) | (u8)reg16;
		}

		reg8 = rcompp - (1 << (srup + 1));
		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			MCHBAR8(rcompctl[0]+0x18+i) =
					(MCHBAR8(rcompctl[0]+0x18+i) & ~0x3f) |
					rcomplut[j][0];
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			if (s->dimm_config[0] < 3 || s->dimm_config[0] == 5) {
				MCHBAR8(rcompctl[2]+0x18+i) =
					(MCHBAR8(rcompctl[2]+0x18+i) & ~0x3f) |
					rcomplut[j][10];
			}
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			MCHBAR8(rcompctl[3]+0x18+i) =
					(MCHBAR8(rcompctl[3]+0x18+i) & ~0x3f) |
					rcomplut[j][6];
			MCHBAR8(rcompctl[4]+0x18+i) =
					(MCHBAR8(rcompctl[4]+0x18+i) & ~0x3f) |
					rcomplut[j][6];
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			MCHBAR8(rcompctl[5]+0x18+i) =
					(MCHBAR8(rcompctl[5]+0x18+i) & ~0x3f) |
					rcomplut[j][8];
			MCHBAR8(rcompctl[6]+0x18+i) =
					(MCHBAR8(rcompctl[6]+0x18+i) & ~0x3f) |
					rcomplut[j][8];
		}

		reg8 = rcompn - (1 << (srun + 1));
		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			MCHBAR8(rcompctl[0]+0x1c+i) =
					(MCHBAR8(rcompctl[0]+0x1c+i) & ~0x3f) |
					rcomplut[j][1];
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			if (s->dimm_config[0] < 3 || s->dimm_config[0] == 5) {
				MCHBAR8(rcompctl[2]+0x1c+i) =
					(MCHBAR8(rcompctl[2]+0x1c+i) & ~0x3f) |
					rcomplut[j][11];
			}
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			MCHBAR8(rcompctl[3]+0x1c+i) =
					(MCHBAR8(rcompctl[3]+0x1c+i) & ~0x3f) |
					rcomplut[j][7];
			MCHBAR8(rcompctl[4]+0x1c+i) =
					(MCHBAR8(rcompctl[4]+0x1c+i) & ~0x3f) |
					rcomplut[j][7];
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			MCHBAR8(rcompctl[5]+0x1c+i) =
					(MCHBAR8(rcompctl[5]+0x1c+i) & ~0x3f) |
					rcomplut[j][9];
			MCHBAR8(rcompctl[6]+0x1c+i) =
					(MCHBAR8(rcompctl[6]+0x1c+i) & ~0x3f) |
					rcomplut[j][9];
		}
	}
	MCHBAR8(0x130) = MCHBAR8(0x130) | 1;
}

static void sdram_odt(struct sysinfo *s)
{
	u8 rankindex = 0;

	static const u16 odt294[16] = {
			0x0000, 0x0000, 0x0000, 0x0000,
			0x0044, 0x1111, 0x0000, 0x1111,
			0x0000, 0x0000, 0x0000, 0x0000,
			0x0044, 0x1111, 0x0000, 0x1111
			};
	static const u16 odt298[16] = {
			0x0000, 0x0011, 0x0000, 0x0011,
			0x0000, 0x4444, 0x0000, 0x4444,
			0x0000, 0x0000, 0x0000, 0x0000,
			0x0000, 0x4444, 0x0000, 0x4444
			};

	switch (s->dimms[0].ranks) {
	case 0:
		if (s->dimms[1].ranks == 0) {
			rankindex = 0;
		} else if (s->dimms[1].ranks == 1) {
			rankindex = 4;
		} else if (s->dimms[1].ranks == 2) {
			rankindex = 12;
		}
		break;
	case 1:
		if (s->dimms[1].ranks == 0) {
			rankindex = 1;
		} else if (s->dimms[1].ranks == 1) {
			rankindex = 5;
		} else if (s->dimms[1].ranks == 2) {
			rankindex = 13;
		}
		break;
	case 2:
		if (s->dimms[1].ranks == 0) {
			rankindex = 3;
		} else if (s->dimms[1].ranks == 1) {
			rankindex = 7;
		} else if (s->dimms[1].ranks == 2) {
			rankindex = 15;
		}
		break;
	}

	MCHBAR16(0x298) = odt298[rankindex];
	MCHBAR16(0x294) = odt294[rankindex];
}

static void sdram_mmap(struct sysinfo *s)
{
	static const u32 w260[7] = {0, 0x400001, 0xc00001, 0x500000, 0xf00000,
				    0xc00001, 0xf00000};
	static const u32 w208[7] = {0, 0x10000, 0x1010000, 0x10001, 0x1010101,
				    0x1010000, 0x1010101};
	static const u32 w200[7] = {0, 0, 0, 0x20002, 0x40002, 0, 0x40002};
	static const u32 w204[7] = {0, 0x20002, 0x40002, 0x40004, 0x80006,
				    0x40002, 0x80006};

	static const u16 tolud[7] = {0x800, 0x800, 0x1000, 0x1000, 0x2000,
				     0x1000, 0x2000};
	static const u16 tom[7] = {0x2, 0x2, 0x4, 0x4, 0x8, 0x4, 0x8};
	static const u16 touud[7] = {0x80, 0x80, 0x100, 0x100, 0x200, 0x100,
				     0x200};
	static const u32 gbsm[7] = {0x8000000, 0x8000000, 0x10000000, 0x8000000,
				    0x20000000, 0x10000000, 0x20000000};
	static const u32 bgsm[7] = {0x8000000, 0x8000000, 0x10000000, 0x8000000,
				    0x20000000, 0x10000000, 0x20000000};
	static const u32 tsegmb[7] = {0x8000000, 0x8000000, 0x10000000,
				      0x8000000, 0x20000000, 0x10000000,
				      0x20000000};

	if ((s->dimm_config[0] < 3) && rank_is_populated(s->dimms, 0, 0)) {
		if (s->dimms[0].sides > 1) {
			// 2R/NC
			MCHBAR32(0x260) = (MCHBAR32(0x260) & ~0x1) | 0x300001;
			MCHBAR32(0x208) = 0x101;
			MCHBAR32(0x200) = 0x40002;
			MCHBAR32(0x204) = w204[s->dimm_config[0]];
		} else {
			// 1R/NC
			MCHBAR32(0x260) = (MCHBAR32(0x260) & ~0x1) | 0x100001;
			MCHBAR32(0x208) = 0x1;
			MCHBAR32(0x200) = 0x20002;
			MCHBAR32(0x204) = w204[s->dimm_config[0]];
		}
	} else if ((s->dimm_config[0] == 5) && rank_is_populated(s->dimms, 0, 0)) {

		MCHBAR32(0x260) = (MCHBAR32(0x260) & ~0x1) | 0x300001;
		MCHBAR32(0x208) = 0x101;
		MCHBAR32(0x200) = 0x40002;
		MCHBAR32(0x204) = 0x40004;
	} else {
		MCHBAR32(0x260) = (MCHBAR32(0x260) & ~0x1) | w260[s->dimm_config[0]];
		MCHBAR32(0x208) = w208[s->dimm_config[0]];
		MCHBAR32(0x200) = w200[s->dimm_config[0]];
		MCHBAR32(0x204) = w204[s->dimm_config[0]];
	}
	pci_write_config16(PCI_DEV(0, 0, 0), 0xb0, tolud[s->dimm_config[0]]);
	pci_write_config16(PCI_DEV(0, 0, 0), 0xa0, tom[s->dimm_config[0]]);
	pci_write_config16(PCI_DEV(0, 0, 0), 0xa2, touud[s->dimm_config[0]]);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xa4, gbsm[s->dimm_config[0]]);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xa8, bgsm[s->dimm_config[0]]);
	pci_write_config32(PCI_DEV(0, 0, 0), 0xac, tsegmb[s->dimm_config[0]]);
}

#if 1
static void hpet_udelay(u32 del)
{
	u32 start, finish, now;

	del *= 15; /* now in usec */

	start = HPET32(0xf0);
	finish = start + del;
	while (1) {
		now = HPET32(0xf0);
		if (finish > start) {
			if (now >= finish)
				break;
		} else {
			if ((now < start) && (now >= finish)) {
				break;
			}
		}
	}
}
#endif

static u8 sdram_checkrcompoverride(void)
{
	u32 xcomp;
	u8 aa, bb, a, b, c, d;

	xcomp = MCHBAR32(0x13c);
	a = (u8)((xcomp & 0x7f000000) >> 24);
	b = (u8)((xcomp & 0x7f0000) >> 16);
	c = (u8)((xcomp & 0x3f00) >> 8);
	d = (u8)(xcomp & 0x3f);

	if (a > b) {
		aa = a - b;
	} else {
		aa = b - a;
	}
	if (c > d) {
		bb = c - d;
	} else {
		bb = d - c;
	}
	if ((aa > 18) || (bb > 7) ||
			(a <= 5) || (b <= 5) || (c <= 5) || (d <= 5) ||
			(a >= 0x7a) || (b >= 0x7a) || (c >= 0x3a) || (d >= 0x3a)) {
		MCHBAR32(0x140) = 0x9718a729;
		return 1;
	}
	return 0;
}

static void sdram_rcompupdate(struct sysinfo *s)
{
	u8 i, ok;
	u32 reg32a, reg32b;

	ok = 0;
	MCHBAR8(0x170) = MCHBAR8(0x170) & ~(1 << 3);
	MCHBAR8(0x130) = MCHBAR8(0x130) & ~(1 << 7);
	for (i = 0; i < 3; i++) {
		MCHBAR8(0x130) = MCHBAR8(0x130) | 1;
		hpet_udelay(1000);
		while ((MCHBAR8(0x130) & 0x1) != 0);
		ok |= sdram_checkrcompoverride();
	}
	if (!ok) {
		reg32a = MCHBAR32(0x13c);
		reg32b = (reg32a >> 16) & 0x0000ffff;
		reg32a = ((reg32a << 16) & 0xffff0000) | reg32b;
		reg32a |= (1 << 31) | (1 << 15);
		MCHBAR32(0x140) = reg32a;
	}
	MCHBAR8(0x130) = MCHBAR8(0x130) | 1;
	hpet_udelay(1000);
	while ((MCHBAR8(0x130) & 0x1) != 0);
}

static void __attribute__((noinline))
sdram_jedec(struct sysinfo *s, u8 rank, u8 jmode, u16 jval)
{
	u32 reg32;

	reg32 = jval << 3;
	reg32 |= rank * 0x8000000;
	MCHBAR8(0x271) = (MCHBAR8(0x271) & ~0x3e) | jmode;
	read32((void *)reg32);
	barrier();
	hpet_udelay(1); // 1us
}

static void sdram_zqcl(struct sysinfo *s)
{
	if (s->boot_path == BOOT_PATH_RESUME) {
		MCHBAR32(0x260) = MCHBAR32(0x260) | (1 << 27);
		MCHBAR8(0x271) = (MCHBAR8(0x271) & ~0xe) | NORMAL_OP_CMD;
		MCHBAR8(0x271) = MCHBAR8(0x271) & ~0x30;
		MCHBAR32(0x268) = (MCHBAR32(0x268) & ~((1 << 30) | (1 << 31))) |
				(1 << 30) | (1 << 31);
	}
}

static void sdram_jedecinit(struct sysinfo *s)
{
	u8 r, i, ch;
	u16 reg16, mrs, rttnom;
	struct jedeclist {
		char debug[15];
		u8 cmd;
		u16 val;
	};

	static const struct jedeclist jedec[12] = {
			{ "   NOP        ", NOP_CMD, 0 },
			{ "   PRE CHARGE ", PRE_CHARGE_CMD, 0 },
			{ "   EMRS2      ", EMRS2_CMD, 0 },
			{ "   EMRS3      ", EMRS3_CMD, 0 },
			{ "   EMRS1      ", EMRS1_CMD, 0 },
			{ "   DLL RESET  ", MRS_CMD, (1 << 8) },
			{ "   PRE CHARGE ", PRE_CHARGE_CMD, 0 },
			{ "   AUTOREFRESH", CBR_CMD, 0 },
			{ "   AUTOREFRESH", CBR_CMD, 0 },
			{ "   INITIALISE ", MRS_CMD, 0 },
			{ "   EMRS1 OCD  ", EMRS1_CMD, (1 << 9) | (1 << 8) | (1 << 7) },
			{ "   EMRS1 EXIT ", EMRS1_CMD, 0 }
	};

	mrs = (s->selected_timings.CAS << 4) |
		((s->selected_timings.tWR - 1) << 9) | (1 << 3) | (1 << 1) | 1;
	rttnom = (1 << 2);
	if (rank_is_populated(s->dimms, 0, 0) && rank_is_populated(s->dimms, 0, 2)) {
		rttnom |= (1 << 6);
	}

	hpet_udelay(200); // 200us
	reg16 = 0;
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		for (i = 0; i < 12; i++) {
			PRINTK_DEBUG("Rank:%d Jedec:%14s...", r, jedec[i].debug);
			reg16 = jedec[i].val;
			switch (jedec[i].cmd) {
			case EMRS1_CMD:
				reg16 |= rttnom;
				break;
			case MRS_CMD:
				reg16 |= mrs;
				break;
			default:
				break;
			}
			sdram_jedec(s, r, jedec[i].cmd, reg16);
			PRINTK_DEBUG("done\n");
		}
	}
}

static void sdram_misc(struct sysinfo *s)
{
	u32 reg32;

	reg32 = 0;
	reg32 |= (0x4 << 13);
	reg32 |= (0x6 << 8);
	MCHBAR32(0x274) = (MCHBAR32(0x274) & ~0x3ff00) | reg32;
	MCHBAR8(0x274) = MCHBAR8(0x274) & ~(1 << 7);
	MCHBAR8(0x26c) = MCHBAR8(0x26c) | 1;
	if (s->boot_path != BOOT_PATH_RESUME) {
		MCHBAR8(0x271) = (MCHBAR8(0x271) & ~0xe) | NORMAL_OP_CMD;
		MCHBAR8(0x271) = MCHBAR8(0x271) & ~0x30;
	} else {
		sdram_zqcl(s);
	}
}

static void sdram_checkreset(void)
{
	u8 pmcon2, pmcon3, reset;

	pmcon2 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);
	pmcon3 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
	pmcon3 &= ~0x2;
	if (pmcon2 & 0x80) {
		pmcon2 &= ~0x80;
		reset = 1;
	} else {
		pmcon2 |= 0x80;
		reset = 0;
	}
	if (pmcon2 & 0x4) {
		pmcon2 |= 0x4;
		pmcon3 = (pmcon3 & ~0x30) | 0x30;
		pmcon3 |= (1 << 3);
	}
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa2, pmcon2);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, pmcon3);
	if (reset) {
		printk(BIOS_DEBUG, "Power cycle reset...\n");
		outb(0xe, 0xcf9);
	}
}

static void sdram_dradrb(struct sysinfo *s)
{
	u8 i, reg8, ch, r;
	u32 reg32, ind, c0dra, c0drb, dra;
	u16 addr;
	i = 0;
	static const u8 dratab[2][2][2][4] =
	{{
		{
		 {0xff, 0xff, 0xff, 0xff},
		 {0xff, 0x00, 0x02, 0xff}
		},
		{
		 {0xff, 0x01, 0xff, 0xff},
		 {0xff, 0x03, 0xff, 0x06}
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
	}};

	static const u8 dradrb[10][6]  =  {
		//Row   Col   Bank  Width         DRB
		{0x01,  0x01,  0x00,  0x08,  0,  0x04},
		{0x01,  0x00,  0x00,  0x10,  0,  0x02},
		{0x02,  0x01,  0x00,  0x08,  1,  0x08},
		{0x01,  0x01,  0x00,  0x10,  1,  0x04},
		{0x01,  0x01,  0x01,  0x08,  1,  0x08},
		{0x00,  0x01,  0x01,  0x10,  1,  0x04},
		{0x02,  0x01,  0x01,  0x08,  2,  0x10},
		{0x01,  0x01,  0x01,  0x10,  2,  0x08},
		{0x03,  0x01,  0x01,  0x08,  3,  0x20},
		{0x02,  0x01,  0x01,  0x10,  3,  0x10},
	};

	reg32 = 0;
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		i = r / 2;
		PRINTK_DEBUG("RANK %d PRESENT\n", r);
		dra = dratab[s->dimms[i].banks]
			[s->dimms[i].width]
			[s->dimms[i].cols - 9]
			[s->dimms[i].rows - 12];

		if (s->dimms[i].banks == 1) {
			dra |= (1 << 7);
		}
		reg32 |= (dra << (r*8));
	}
	MCHBAR32(0x208) = reg32;
	c0dra = reg32;
	PRINTK_DEBUG("C0DRA = 0x%08x\n", c0dra);

	reg32 = 0;
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		reg32 |= (1 << r);
	}
	reg8 = (u8)(reg32 << 4) & 0xf0;
	MCHBAR8(0x262) = (MCHBAR8(0x262) & ~0xf0) | reg8;
	if (ONLY_DIMMA_IS_POPULATED(s->dimms, 0) ||
			ONLY_DIMMB_IS_POPULATED(s->dimms, 0)) {
		MCHBAR8(0x260) = MCHBAR8(0x260) | 1;
	}

	addr = 0x200;
	c0drb = 0;
	FOR_EACH_RANK(ch, r) {
		if (rank_is_populated(s->dimms, ch, r)) {
			ind = (c0dra >> (8*r)) & 0x7f;
			c0drb = (u16)(c0drb + dradrb[ind][5]);
			s->channel_capacity[0] += dradrb[ind][5] << 6;
		}
		MCHBAR16(addr) = c0drb;
		addr += 2;
	}
	printk(BIOS_DEBUG, "Total memory = %dMB\n", s->channel_capacity[0]);
}

static u8 sampledqs(u32 dqshighaddr, u32 strobeaddr, u8 highlow, u8 count)
{
	volatile u32 strobedata;
	u8 dqsmatches = 1;
	while (count--) {
		MCHBAR8(0x5d8) = MCHBAR8(0x5d8) & ~0x2;
		hpet_udelay(1);
		MCHBAR8(0x5d8) = MCHBAR8(0x5d8) | 0x2;
		hpet_udelay(1);
		barrier();
		strobedata = read32((void *)strobeaddr);
		barrier();
		hpet_udelay(1);

		if (((MCHBAR8(dqshighaddr) & 0x40) >> 6) != highlow) {
			dqsmatches = 0;
		}
	}

	return dqsmatches;
}

static void rcvenclock(u8 *coarse, u8 *medium, u8 bytelane)
{
	if (*medium < 3) {
		(*medium)++;
		MCHBAR16(0x58c) = (MCHBAR16(0x58c) & (u16)(~(0x3 << (bytelane*2))))
					| (*medium << (bytelane*2));
	} else {
		*medium = 0;
		(*coarse)++;
		MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0xf0000) | (*coarse << 16);
		MCHBAR16(0x58c) = (MCHBAR16(0x58c) & (u16)(~0x3 << (bytelane*2)))
					| (*medium << (bytelane*2));
	}
}

static void sdram_rcven(struct sysinfo *s)
{
	u8 curcoarse, savecoarse;
	u8 curmedium, savemedium;
	u8 pi, savepi;
	u8 bytelane;
	u8 bytelanecoarse[8] = { 0 };
	u8 minbytelanecoarse = 0xff;
	u8 bytelaneoffset;
	u8 maxbytelane = 8;
	u32 strobeaddr = (rank_is_populated(s->dimms, 0, 0)) ? 0 : 2*128*1024*1024;
	u32 dqshighaddr;

	MCHBAR8(0x5d8) = MCHBAR8(0x5d8) & ~0xc;
	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) & ~0x80;

	PRINTK_DEBUG("rcven 0\n");
	for (bytelane = 0; bytelane < maxbytelane; bytelane++) {
		PRINTK_DEBUG("rcven bytelane %d\n", bytelane);
//trylaneagain:
		dqshighaddr = 0x561 + (bytelane << 2);

		curcoarse = s->selected_timings.CAS + 1;
		pi = 0;
		curmedium = 0;

		MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0xf0000) | (curcoarse << 16);
		MCHBAR16(0x58c) = (MCHBAR16(0x58c) & (u16)(~(0x3 << (bytelane*2))))
						| (curmedium << (bytelane*2));
		MCHBAR8(0x560+bytelane*4) = MCHBAR8(0x560+bytelane*4) & ~0x3f;

		savecoarse = curcoarse;
		savemedium = curmedium;
		savepi = pi;

		PRINTK_DEBUG("rcven 0.1\n");

		//MCHBAR16(0x588) = (MCHBAR16(0x588) & (u16)~(0x3 << (bytelane*2))) | (1 << (bytelane*2)); // XXX comment out

		while (sampledqs(dqshighaddr, strobeaddr, 0, 3) == 0) {
			//printk(BIOS_DEBUG, "coarse=%d medium=%d\n", curcoarse, curmedium);
			rcvenclock(&curcoarse, &curmedium, bytelane);
			if (curcoarse > 0xf) {
				PRINTK_DEBUG("Error: coarse > 0xf\n");
				//goto trylaneagain;
				break;
			}
		}
		PRINTK_DEBUG("rcven 0.2\n");

		savecoarse = curcoarse;
		savemedium = curmedium;
		rcvenclock(&curcoarse, &curmedium, bytelane);

		while (sampledqs(dqshighaddr, strobeaddr, 1, 3) == 0) {
			savecoarse = curcoarse;
			savemedium = curmedium;
			rcvenclock(&curcoarse, &curmedium, bytelane);
			if (curcoarse > 0xf) {
				PRINTK_DEBUG("Error: coarse > 0xf\n");
				//goto trylaneagain;
				break;
			}
		}

		PRINTK_DEBUG("rcven 0.3\n");
		curcoarse = savecoarse;
		curmedium = savemedium;
		MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0xf0000) | (curcoarse << 16);
		MCHBAR16(0x58c) = (MCHBAR16(0x58c) & (u16)(~(0x3 << bytelane*2)))
						| (curmedium << (bytelane*2));

		while (sampledqs(dqshighaddr, strobeaddr, 1, 3) == 0) {
			savepi = pi;
			pi++;
			if (pi > s->maxpi) {
				//if (s->nodll) {
					pi = savepi = s->maxpi;
					break;
				//}
			}
			MCHBAR8(0x560 + bytelane*4) = (MCHBAR8(0x560 + bytelane*4)
					& ~0x3f) | (pi << s->pioffset);
		}
		PRINTK_DEBUG("rcven 0.4\n");

		pi = savepi;
		MCHBAR8(0x560 + bytelane*4) = (MCHBAR8(0x560 + bytelane*4) & ~0x3f)
						| (pi << s->pioffset);
		rcvenclock(&curcoarse, &curmedium, bytelane);
		if (sampledqs(dqshighaddr, strobeaddr, 1, 1) == 0) {
			PRINTK_DEBUG("Error: DQS not high\n");
			//goto trylaneagain;
		}
		PRINTK_DEBUG("rcven 0.5\n");
		while (sampledqs(dqshighaddr, strobeaddr, 0, 3) == 0) {
			curcoarse--;
			MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0xf0000)
						| (curcoarse << 16);
			if (curcoarse == 0) {
				PRINTK_DEBUG("Error: DQS did not hit 0\n");
				break;
			}
		}

		PRINTK_DEBUG("rcven 0.6\n");
		rcvenclock(&curcoarse, &curmedium, bytelane);
		s->pi[bytelane] = pi;
		bytelanecoarse[bytelane] = curcoarse;
	}

	PRINTK_DEBUG("rcven 1\n");

	bytelane = maxbytelane;
	do {
		bytelane--;
		if (minbytelanecoarse > bytelanecoarse[bytelane]) {
			minbytelanecoarse = bytelanecoarse[bytelane];
		}
	} while (bytelane != 0);

	bytelane = maxbytelane;
	do {
		bytelane--;
		bytelaneoffset = bytelanecoarse[bytelane] - minbytelanecoarse;
		MCHBAR16(0x5fa) = (MCHBAR16(0x5fa) & (u16)(~(0x3 << (bytelane*2))))
					| (bytelaneoffset << (bytelane*2));
	} while (bytelane != 0);

	MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0xf0000) | (minbytelanecoarse << 16);

	s->coarsectrl = minbytelanecoarse;
	s->coarsedelay = MCHBAR16(0x5fa);
	s->mediumphase = MCHBAR16(0x58c);
	s->readptrdelay = MCHBAR16(0x588);

	PRINTK_DEBUG("rcven 2\n");
	MCHBAR8(0x5d8) = MCHBAR8(0x5d8) & ~0xe;
	MCHBAR8(0x5d8) = MCHBAR8(0x5d8) | 0x2;
	MCHBAR8(0x5d8) = MCHBAR8(0x5d8) | 0x4;
	MCHBAR8(0x5d8) = MCHBAR8(0x5d8) | 0x8;

	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) | 0x80;
	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) & ~0x80;
	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) | 0x80;
	PRINTK_DEBUG("rcven 3\n");
}

static void sdram_mmap_regs(struct sysinfo *s)
{
	bool reclaim;
	u32 tsegsize;
	u32 mmiosize;
	u32 tom, tolud, touud, reclaimbase, reclaimlimit;
	u32 gfxbase, gfxsize, gttbase, gttsize, tsegbase;
	u16 ggc;
	u16 ggc_to_uma[10] = { 0, 1, 4, 8, 16, 32, 48, 64, 128, 256 };
	u8 ggc_to_gtt[4] = { 0, 1, 0, 0 };

	reclaimbase = 0;
	reclaimlimit = 0;
	ggc = pci_read_config16(PCI_DEV(0,0,0), GGC);
	printk(BIOS_DEBUG, "GGC = 0x%04x\n", ggc);
	gfxsize = ggc_to_uma[(ggc & 0xf0) >> 4];
	gttsize = ggc_to_gtt[(ggc & 0x300) >> 8];
	tom = s->channel_capacity[0];

	tsegsize = 0x8; // 8MB
	mmiosize = 0x400; // 1GB

	reclaim = false;
	tolud = MIN(0x1000 - mmiosize, tom);
	if ((tom - tolud) > 0x40) {
		reclaim = true;
	}
	if (reclaim) {
		tolud = tolud & ~0x3f;
		tom = tom & ~0x3f;
		reclaimbase = MAX(0x1000, tom);
		reclaimlimit = reclaimbase + (MIN(0x1000, tom) - tolud) - 0x40;
	}
	touud = tom;
	if (reclaim) {
		touud = reclaimlimit + 0x40;
	}

	gfxbase = tolud - gfxsize;
	gttbase = gfxbase - gttsize;
	tsegbase = gttbase - tsegsize;

	/* Program the regs */
	pci_write_config16(PCI_DEV(0,0,0), TOLUD, (u16)(tolud << 4));
	pci_write_config16(PCI_DEV(0,0,0), TOM, (u16)(tom >> 6));
	if (reclaim) {
		pci_write_config16(PCI_DEV(0,0,0), 0x98, (u16)(reclaimbase >> 6));
		pci_write_config16(PCI_DEV(0,0,0), 0x9a, (u16)(reclaimlimit >> 6));
	}
	pci_write_config16(PCI_DEV(0,0,0), TOUUD, (u16)(touud));
	pci_write_config32(PCI_DEV(0,0,0), GBSM, gfxbase << 20);
	pci_write_config32(PCI_DEV(0,0,0), BGSM, gttbase << 20);
	pci_write_config32(PCI_DEV(0,0,0), TSEG, tsegbase << 20);

	u8 reg8 = pci_read_config8(PCI_DEV(0, 0, 0), ESMRAMC);
	reg8 &= ~0x7;
	reg8 |= (2 << 1) | (1 << 0); /* 8M and TSEG_Enable */
	pci_write_config8(PCI_DEV(0, 0, 0), ESMRAMC, reg8);

	printk(BIOS_DEBUG, "GBSM (igd) = verified %08x (written %08x)\n",
		pci_read_config32(PCI_DEV(0,0,0), GBSM), gfxbase << 20);
	printk(BIOS_DEBUG, "BGSM (gtt) = verified %08x (written %08x)\n",
		pci_read_config32(PCI_DEV(0,0,0), BGSM), gttbase << 20);
	printk(BIOS_DEBUG, "TSEG (smm) = verified %08x (written %08x)\n",
		pci_read_config32(PCI_DEV(0,0,0), TSEG), tsegbase << 20);
}

static void sdram_enhancedmode(struct sysinfo *s)
{
	u8 reg8, ch, r, j, i;
	u32 mask32, reg32;
	MCHBAR8(0x246) = MCHBAR8(0x246) | 1;
	MCHBAR8(0x269 + 3) = MCHBAR8(0x269 + 3) | 1;
	mask32 = (0x1f << 15) | (0x1f << 10) | (0x1f << 5) | 0x1f;
	reg32 = (0x1e << 15) | (0x10 << 10) | (0x1e << 5) | 0x10;
	MCHBAR32(0x120) = (MCHBAR32(0x120) & ~mask32) | reg32;
	MCHBAR8(0x288 + 1) = 0x2;
	MCHBAR16(0x288 + 2) = 0x0804;
	MCHBAR16(0x288 + 4) = 0x2010;
	MCHBAR8(0x288 + 6) = 0x40;
	MCHBAR16(0x288 + 8) = 0x091c;
	MCHBAR8(0x288 + 10) = 0xf2;
	MCHBAR8(0x241) = MCHBAR8(0x241) | 1;
	MCHBAR8(0x243) = MCHBAR8(0x243) | 1;
	MCHBAR16(0x272) = MCHBAR16(0x272) | 0x100;

	reg8 = pci_read_config8(PCI_DEV(0,0,0), 0xf0);
	pci_write_config8(PCI_DEV(0,0,0), 0xf0, reg8 | 1);
	MCHBAR32(0xfa0) = 0x00000002;
	MCHBAR32(0xfa4) = 0x20310002;
	MCHBAR32(0x24) = 0x02020302;
	MCHBAR32(0x30) = 0x001f1806;
	MCHBAR32(0x34) = 0x01102800;
	MCHBAR32(0x38) = 0x07000000;
	MCHBAR32(0x3c) = 0x01014010;
	MCHBAR32(0x40) = 0x0f038000;
	reg8 = pci_read_config8(PCI_DEV(0,0,0), 0xf0);
	pci_write_config8(PCI_DEV(0,0,0), 0xf0, reg8 & ~1);

	u32 nranks, curranksize, maxranksize, maxdra, dra;
	u8 rankmismatch, dramismatch;
	static const u8 drbtab[10] = { 0x4, 0x2, 0x8, 0x4, 0x8, 0x4, 0x10, 0x8,
				       0x20, 0x10 };

	nranks = 0;
	curranksize = 0;
	maxranksize = 0;
	maxdra = 0;
	rankmismatch = 0;
	dramismatch = 0;
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		nranks++;
		dra = (u8) ((MCHBAR32(0x208) >> (8*r)) & 0x7f);
		curranksize = drbtab[dra];
		if (maxranksize == 0) {
			maxranksize = curranksize;
			maxdra = dra;
		}
		if (curranksize != maxranksize) {
			rankmismatch = 1;
		}
		if (dra != maxdra) {
			dramismatch = 1;
		}
	}

	reg8 = 0;
	switch (nranks) {
	case 4:
		if (rankmismatch) {
			reg8 = 0x64;
		} else {
			reg8 = 0xa4;
		}
		break;
	case 1:
	case 3:
		reg8 = 0x64;
		break;
	case 2:
		if (rankmismatch) {
			reg8 = 0x64;
		} else {
			reg8 = 0x24;
		}
		break;
	default:
		die("Invalid number of ranks found, halt\n");
		break;
	}
	MCHBAR8(0x111) = (MCHBAR8(0x111) & ~0xfc) | (reg8 & 0xfc);
	MCHBAR32(0xd0) = MCHBAR32(0xd0) & ~0x80000000;

	MCHBAR32(0x28) = 0xf;
	MCHBAR8(0x2c4) = MCHBAR8(0x2c4) | 1;

	MCHBAR32(0x3c) = MCHBAR32(0x3c) & ~0xe000000;
	MCHBAR32(0x40) = (MCHBAR32(0x40) & ~0xc0000) | 0x40000;
	u32 clkcx[2][2][3] = {
				{
					{0, 0x0c080302, 0x08010204},	// 667
					{0x02040000, 0x08100102, 0}
				},
				{
					{0x18000000, 0x3021060c, 0x20010208},
					{0, 0x0c090306, 0}		// 800
				}
			};
	j = s->selected_timings.fsb_clock;
	i = s->selected_timings.mem_clock;

	MCHBAR32(0x708) = clkcx[j][i][0];
	MCHBAR32(0x70c) = clkcx[j][i][1];
	MCHBAR32(0x6dc) = clkcx[j][i][2];
	MCHBAR8(0x40) = MCHBAR8(0x40) & ~0x2;
}

static void sdram_periodic_rcomp(void)
{
	MCHBAR8(0x130) = MCHBAR8(0x130) & ~0x2;
	while ((MCHBAR32(0x130) & 0x80000000) > 0) {
		;
	}
	MCHBAR16(0x1b4) = (MCHBAR16(0x1b4) & ~0x3000);

	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) | 0x80;
	MCHBAR16(0x170) = (MCHBAR16(0x170) & ~0xf) | 0x9;

	MCHBAR8(0x130) = MCHBAR8(0x130) | 0x82;
}

static void sdram_new_trd(struct sysinfo *s)
{
	u8 pidelay, i, j, k, cc, trd_perphase[5];
	u8 bypass, freqgb, trd, reg8, txfifo, cas;
	u32 reg32, datadelay, tio, rcvendelay, maxrcvendelay;
	u16 tmclk, thclk, buffertocore, postcalib;
	static const u8 txfifo_lut[8] = { 0, 7, 6, 5, 2, 1, 4, 3 };
	static const u16 trd_adjust[2][2][5] = {
			{
				{3000, 3000, 0,0,0},
				{1000,2000,3000,1500,2500}
			},
			{
				{2000,1000,3000,0,0},
				{2500, 2500, 0,0,0}
			}};

	freqgb = 110;
	buffertocore = 5000;
	cas = s->selected_timings.CAS;
	postcalib = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 1250 : 500;
	tmclk = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 3000 : 2500;
	tmclk = tmclk * 100 / freqgb;
	thclk = (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) ? 6000 : 5000;
	switch (s->selected_timings.mem_clock) {
	case MEM_CLOCK_667MHz:
		if (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) {
			cc = 2;
		} else {
			cc = 3;
		}
		break;
	default:
	case MEM_CLOCK_800MHz:
		if (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) {
			cc = 5;
		} else {
			cc = 2;
		}
		break;
	}
	tio = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 2700 : 3240;
	maxrcvendelay = 0;
	pidelay = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 24 : 20;

	for (i = 0; i < 8; i++) {
		rcvendelay = ((u32)((s->coarsedelay >> (i << 1)) & 0x3) * (u32)(tmclk));
		rcvendelay += ((u32)((s->readptrdelay >> (i << 1)) & 0x3) * (u32)(tmclk) / 2);
		rcvendelay += ((u32)((s->mediumphase >> (i << 1)) & 0x3) * (u32)(tmclk) / 4);
		rcvendelay += (u32)(pidelay * s->pi[i]);
		maxrcvendelay = MAX(maxrcvendelay, rcvendelay);
	}

	if ((MCHBAR8(0xc54+3) == 0xff) && (MCHBAR8(0xc08) & 0x80)) {
		bypass = 1;
	} else {
		bypass = 0;
	}

	txfifo = 0;
	reg8 = (MCHBAR8(0x188) & 0xe) >> 1;
	txfifo = txfifo_lut[reg8] & 0x7;

	datadelay = tmclk * (2*txfifo + 4*s->coarsectrl + 4*(bypass-1) + 13) / 4
			+ tio + maxrcvendelay + pidelay + buffertocore + postcalib;
	if (s->async) {
		datadelay += tmclk / 2;
	}

	j = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 0 : 1;
	k = (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) ? 0 : 1;

	if (j == 0 && k == 0) {
		datadelay -= 3084;
	}

	trd = 0;
	for (i = 0; i < cc; i++) {
		reg32 = datadelay - (trd_adjust[k][j][i] * 100 / freqgb);
		trd_perphase[i] = (u8)(reg32 / thclk) - 2;
		trd_perphase[i] += 1;
		if (trd_perphase[i] > trd) {
			trd = trd_perphase[i];
		}
	}

	MCHBAR16(0x248) = (MCHBAR16(0x248) & ~0x1f00) | (trd << 8);
}

static void sdram_powersettings(struct sysinfo *s)
{
	u8 j;
	u32 reg32;

	/* Thermal sensor */
	MCHBAR8(0x3808) = 0x9b;
	MCHBAR32(0x380c) = (MCHBAR32(0x380c) & ~0x00ffffff) | 0x1d00;
	MCHBAR8(0x3814) = 0x08;
	MCHBAR8(0x3824) = 0x00;
	MCHBAR8(0x3809) = (MCHBAR8(0x3809) & ~0xf) | 0x4;
	MCHBAR8(0x3814) = (MCHBAR8(0x3814) & ~1) | 1;
	MCHBAR8(0x3812) = (MCHBAR8(0x3812) & ~0x80) | 0x80;

	/* Clock gating */
	MCHBAR32(0xf18) = MCHBAR32(0xf18) & ~0x00040001;
	MCHBAR8(0xfac+3) = MCHBAR8(0xfac+3) & ~0x80;
	MCHBAR8(0xff8+3) = MCHBAR8(0xff8+3) & ~0x80;
	MCHBAR16(0xff0) = MCHBAR16(0xff0) & ~0x1fff;
	MCHBAR32(0xfb0) = MCHBAR32(0xfb0) & ~0x0001ffff;
	MCHBAR16(0x48) = (MCHBAR16(0x48) & ~0x03ff) & 0x6;
	MCHBAR32(0x20) = (MCHBAR32(0x20) & ~0xffffffff) | 0x20;
	MCHBAR8(0xd14) = MCHBAR8(0xd14) & ~1;
	MCHBAR8(0x239) = s->selected_timings.CAS - 1 + 0x15;
	MCHBAR16(0x2d1) = (MCHBAR16(0x2d1) & ~0x07fc) | 0x40;
	MCHBAR16(0x6d1) = (MCHBAR16(0x6d1) & ~0x0fff) | 0xd00;
	MCHBAR16(0x210) = MCHBAR16(0x210) & ~0x0d80;
	MCHBAR16(0xf6c+2) = 0xffff;

	/* Sequencing */
	MCHBAR32(0x14) = (MCHBAR32(0x14) & ~0x1fffffff) | 0x1f643fff;
	MCHBAR32(0x18) = (MCHBAR32(0x18) & ~0xffffff7f) | 0x02010000;
	MCHBAR16(0x1c) = (MCHBAR16(0x1c) & ~0x7000) | (0x3 << 12);

	/* Power */
	MCHBAR32(0x1104) = (MCHBAR32(0x1104) & ~0xffff0003) | 0x10100000;
	MCHBAR32(0x1108) = (MCHBAR32(0x1108) & ~0x0001bff7) | 0x00000078;
	if (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) {
		MCHBAR16(0x110c) = (MCHBAR16(0x110c) & ~0x03ff) | 0xc8;
	} else {
		MCHBAR16(0x110c) = (MCHBAR16(0x110c) & ~0x03ff) | 0x100;
	}
	j = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 0 : 1;

	MCHBAR32(0x1110) = (MCHBAR32(0x1110) & ~0x1fff37f) | 0x10810700;
	MCHBAR8(0x1114) = (MCHBAR8(0x1114) & ~0x07) | 1;
	MCHBAR8(0x1124) = MCHBAR8(0x1124) & ~0x02;

	static const u16 ddr2lut[2][4][2] = {{
					{0x0000,  0x0000},
					{0x019A,  0x0039},
					{0x0099,  0x1049},
					{0x0000,  0x0000}
				},
				{
					{0x0000,  0x0000},
					{0x019A,  0x0039},
					{0x0099,  0x1049},
					{0x0099,  0x2159}
				}};

	MCHBAR16(0x23c) = 0x7a89;
	MCHBAR8(0x117) = 0xaa;
	MCHBAR16(0x118) = ddr2lut[j][s->selected_timings.CAS - 3][1];
	MCHBAR16(0x115) = (MCHBAR16(0x115) & ~0x7fff) | ddr2lut[j]
		[s->selected_timings.CAS - 3][0];
	MCHBAR16(0x210) = (MCHBAR16(0x210) & ~0xf000) | 0xf000;
	MCHBAR8(0x2c02) = (MCHBAR8(0x2c02) & ~0x77) | (4 << 4 | 4);
	if (s->nodll) {
		reg32 = 0x30000000;
	} else {
		reg32 = 0;
	}
	MCHBAR32(0x2c0) = (MCHBAR32(0x2c0) & ~0x0f000000) | 0x20000000 | reg32;
	MCHBAR32(0x2d1) = (MCHBAR32(0x2d1) & ~0x00f00000) | 0x00f00000;
	MCHBAR32(0x6d0) = (MCHBAR32(0x6d0) & ~0x001ff000) | (0xbf << 20);
	MCHBAR16(0x610) = (MCHBAR16(0x610) & ~0x1f7f) | (0xb << 8) | (7 << 4) | 0xb;
	MCHBAR16(0x612) = 0x3264;
	MCHBAR16(0x614) = (MCHBAR16(0x614) & ~0x3f3f) | (0x14 << 8) | 0xa;

	MCHBAR32(0x6c0) = MCHBAR32(0x6c0) | 0x80002000;
}

static void sdram_programddr(void)
{
	MCHBAR16(0x6d1) = (MCHBAR16(0x6d1) & ~0x03ff) | 0x100;
	MCHBAR16(0x210) = (MCHBAR16(0x210) & ~0x003f) | 0x10;
	MCHBAR16(0x2d1) = (MCHBAR16(0x2d1) & ~0x7000) | 0x2000;
	MCHBAR8(0x180) = MCHBAR8(0x180) & ~0xe;
	MCHBAR8(0x18c) = MCHBAR8(0x18c) & ~0xc;
	MCHBAR8(0x561) = MCHBAR8(0x561) & ~0xe;
	MCHBAR8(0x565) = MCHBAR8(0x565) & ~0xe;
	MCHBAR8(0x569) = MCHBAR8(0x569) & ~0xe;
	MCHBAR8(0x56d) = MCHBAR8(0x56d) & ~0xe;
	MCHBAR8(0x571) = MCHBAR8(0x571) & ~0xe;
	MCHBAR8(0x575) = MCHBAR8(0x575) & ~0xe;
	MCHBAR8(0x579) = MCHBAR8(0x579) & ~0xe;
	MCHBAR8(0x57d) = MCHBAR8(0x57d) & ~0xe;
	MCHBAR8(0x18c) = MCHBAR8(0x18c) & ~0x2;
	MCHBAR16(0x1b4) = MCHBAR16(0x1b4) & ~0x400;
	MCHBAR16(0x210) = MCHBAR16(0x210) & ~0xdc0;
	MCHBAR8(0x239) = MCHBAR8(0x239) & ~0x80;
	MCHBAR32(0x2c0) = MCHBAR32(0x2c0) & ~(1 << 22);
	MCHBAR16(0x2d1) = MCHBAR16(0x2d1) & ~0x80fc;
	MCHBAR16(0x6d1) = MCHBAR16(0x6d1) & ~0xc00;
	MCHBAR8(0x180) = MCHBAR8(0x180) & ~0xd;
	MCHBAR8(0x561) = MCHBAR8(0x561) & ~1;
	MCHBAR8(0x565) = MCHBAR8(0x565) & ~1;
	MCHBAR8(0x569) = MCHBAR8(0x569) & ~1;
	MCHBAR8(0x56d) = MCHBAR8(0x56d) & ~1;
	MCHBAR8(0x571) = MCHBAR8(0x571) & ~1;
	MCHBAR8(0x575) = MCHBAR8(0x575) & ~1;
	MCHBAR8(0x579) = MCHBAR8(0x579) & ~1;
	MCHBAR8(0x57d) = MCHBAR8(0x57d) & ~1;
	MCHBAR32(0x248) = (MCHBAR32(0x248) & ~0x700000) | (0x3 << 20);
	MCHBAR32(0x2c0) = MCHBAR32(0x2c0) & ~0x100000;
	MCHBAR8(0x592) = MCHBAR8(0x592) | 0x1e;
	MCHBAR8(0x2c15) = MCHBAR8(0x2c15) | 0x3;
	MCHBAR32(0x62c) = (MCHBAR32(0x62c) & ~0xc000000) | 0x4000000;
	MCHBAR16(0x248) = MCHBAR16(0x248) | 0x6000;
	MCHBAR32(0x260) = MCHBAR32(0x260) | 0x10000;
	MCHBAR8(0x2c0) = MCHBAR8(0x2c0) | 0x10;
	MCHBAR32(0x2d0) = MCHBAR32(0x2d0) | (0xf << 24);
	MCHBAR8(0x189) = MCHBAR8(0x189) | 0x7;
	MCHBAR8(0x592) = MCHBAR8(0x592) | 0xc0;
	MCHBAR8(0x124) = MCHBAR8(0x124) | 0x7;
	MCHBAR16(0x12a) = (MCHBAR16(0x12a) & ~0xffff) | 0x0080;
	MCHBAR8(0x12c) = (MCHBAR8(0x12c) & ~0xff) | 0x10;
	MCHBAR16(0x2c0) = MCHBAR16(0x2c0) | 0x1e0;
	MCHBAR8(0x189) = MCHBAR8(0x189) | 0x18;
	MCHBAR8(0x193) = MCHBAR8(0x193) | 0xd;
	MCHBAR16(0x212) = MCHBAR16(0x212) | 0xa3f;
	MCHBAR8(0x248) = MCHBAR8(0x248) | 0x3;
	MCHBAR8(0x268) = (MCHBAR8(0x268) & ~0xff) | 0x4a;
	MCHBAR8(0x2c4) = MCHBAR8(0x2c4) & ~0x60;
	MCHBAR16(0x592) = MCHBAR16(0x592) | 0x321;
}

static void sdram_programdqdqs(struct sysinfo *s)
{
	u16 mdclk, tpi, refclk, dqdqs_out, dqdqs_outdelay, dqdqs_delay;
	u32 coretomcp, txdelay, tmaxunmask, tmaxpi;
	u8 repeat, halfclk, feature, reg8, push;
	u16 cwb, pimdclk;
	u32 reg32;
	static const u8 txfifotab[8] = { 0, 7, 6, 5, 2, 1, 4, 3 };

	tpi = 3000;
	dqdqs_out = 4382;
	dqdqs_outdelay = 5083;
	dqdqs_delay = 4692;
	coretomcp = 0;
	txdelay = 0;
	halfclk = 0;
	tmaxunmask = 0;
	tmaxpi = 0;
	repeat = 2;
	feature = 0;
	cwb = 0;
	pimdclk = 0;
	reg32 = 0;
	push = 0;
	reg8 = 0;

	mdclk = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 3000 : 2500;
	refclk = 3000 - mdclk;

	coretomcp = ((MCHBAR8(0x246) >> 2) & 0x3) + 1;
	coretomcp *= mdclk;

	reg8 = (MCHBAR8(0x188) & 0xe) >> 1;

	while (repeat) {
		txdelay = mdclk * (
				((MCHBAR16(0x220) >> 8) & 0x7) +
				(MCHBAR8(0x24d) & 0xf) +
				(MCHBAR8(0x24e) & 0x1)
				) +
				txfifotab[reg8]*(mdclk/2) +
				coretomcp +
				refclk +
				cwb;
		halfclk = (MCHBAR8(0x5d9) >> 1) & 0x1;
		if (halfclk) {
			txdelay -= mdclk / 2;
			reg32 = dqdqs_outdelay + coretomcp - mdclk / 2;
		} else {
			reg32 = dqdqs_outdelay + coretomcp;
		}

		tmaxunmask = txdelay - mdclk - dqdqs_out;
		tmaxpi = tmaxunmask - tpi;

		if ((tmaxunmask >= reg32) && tmaxpi >= dqdqs_delay) {
			if (repeat == 2) {
				MCHBAR32(0x2c0) = MCHBAR32(0x2c0) & ~(1 << 23);
			}
			feature = 1;
			repeat = 0;
		} else {
			repeat--;
			MCHBAR32(0x2c0) = MCHBAR32(0x2c0) | (1 << 23);
			cwb = 2 * mdclk;
		}
	}

	if (!feature) {
		MCHBAR8(0x2d1) = MCHBAR8(0x2d1) & ~0x3;
		return;
	}
	MCHBAR8(0x2d1) = MCHBAR8(0x2d1) | 0x3;
	MCHBAR16(0x210) = (MCHBAR16(0x210) & ~0xf000) | (pimdclk << 12);
	MCHBAR8(0x2c02) = (MCHBAR8(0x2c02) & ~0x77) | (push << 4) | push;
	MCHBAR32(0x2c0) = (MCHBAR32(0x2c0) & ~0xf000000) | 0x3000000;
}

/**
 * @param boot_path: 0 = normal, 1 = reset, 2 = resume from s3
 */
void sdram_initialize(int boot_path, const u8 *spd_addresses)
{
	struct sysinfo si;
	u8 reg8;
	const char *boot_str[] = { "Normal", "Reset", "Resume"};

	PRINTK_DEBUG("Setting up RAM controller.\n");

	memset(&si, 0, sizeof(si));

	si.boot_path = boot_path;
	printk(BIOS_DEBUG, "Boot path: %s\n", boot_str[boot_path]);
	si.spd_map[0] = spd_addresses[0];
	si.spd_map[1] = spd_addresses[1];
	si.spd_map[2] = spd_addresses[2];
	si.spd_map[3] = spd_addresses[3];

	sdram_read_spds(&si);

	/* Choose Common Frequency */
	sdram_detect_ram_speed(&si);

	/* Determine smallest common tRAS, tRP, tRCD, etc */
	sdram_detect_smallest_params(&si);

	/* Enable HPET */
	enable_hpet();

	MCHBAR16(0xc1c) = MCHBAR16(0xc1c) | (1 << 15);

	sdram_clk_crossing(&si);

	sdram_checkreset();
	PRINTK_DEBUG("Done checkreset\n");

	sdram_clkmode(&si);
	PRINTK_DEBUG("Done clkmode\n");

	sdram_timings(&si);
	PRINTK_DEBUG("Done timings (dqs dll enabled)\n");

	if (si.boot_path != BOOT_PATH_RESET) {
		sdram_dlltiming(&si);
		PRINTK_DEBUG("Done dlltiming\n");
	}

	if (si.boot_path != BOOT_PATH_RESET) {
		sdram_rcomp(&si);
		PRINTK_DEBUG("Done RCOMP\n");
	}

	sdram_odt(&si);
	PRINTK_DEBUG("Done odt\n");

	if (si.boot_path != BOOT_PATH_RESET) {
		while ((MCHBAR8(0x130) & 0x1) != 0)
			;
	}

	sdram_mmap(&si);
	PRINTK_DEBUG("Done mmap\n");

	// Enable DDR IO buffer
	MCHBAR8(0x5dd) = (MCHBAR8(0x5dd) & ~0x3f) | 0x8;
	MCHBAR8(0x5d8) = MCHBAR8(0x5d8) | 0x1;

	sdram_rcompupdate(&si);
	PRINTK_DEBUG("Done RCOMP update\n");

	MCHBAR8(0x40) = MCHBAR8(0x40) | 0x2;

	if (si.boot_path != BOOT_PATH_RESUME) {
		MCHBAR32(0x260) = MCHBAR32(0x260) | (1 << 27);

		sdram_jedecinit(&si);
		PRINTK_DEBUG("Done MRS\n");
	}

	sdram_misc(&si);
	PRINTK_DEBUG("Done misc\n");

	sdram_zqcl(&si);
	PRINTK_DEBUG("Done zqcl\n");

	if (si.boot_path != BOOT_PATH_RESUME) {
		MCHBAR32(0x268) = MCHBAR32(0x268) | 0xc0000000;
	}

	sdram_dradrb(&si);
	PRINTK_DEBUG("Done dradrb\n");

	sdram_rcven(&si);
	PRINTK_DEBUG("Done rcven\n");

	sdram_new_trd(&si);
	PRINTK_DEBUG("Done tRD\n");

	sdram_mmap_regs(&si);
	PRINTK_DEBUG("Done mmap regs\n");

	sdram_enhancedmode(&si);
	PRINTK_DEBUG("Done enhanced mode\n");

	sdram_powersettings(&si);
	PRINTK_DEBUG("Done power settings\n");

	sdram_programddr();
	PRINTK_DEBUG("Done programming ddr\n");

	sdram_programdqdqs(&si);
	PRINTK_DEBUG("Done programming dqdqs\n");

	sdram_periodic_rcomp();
	PRINTK_DEBUG("Done periodic RCOMP\n");

	/* Set init done */
	MCHBAR32(0x268) = MCHBAR32(0x268) | 0x40000000;

	/* Tell ICH7 that we're done */
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa2, reg8 & ~0x80);

	/* Tell northbridge we're done */
	reg8 = pci_read_config8(PCI_DEV(0,0,0), 0xf4);
	pci_write_config8(PCI_DEV(0,0,0), 0xf4, reg8 | 1);

	printk(BIOS_DEBUG, "RAM initialization finished.\n");
}
