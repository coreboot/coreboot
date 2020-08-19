/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cf9_reset.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <lib.h>
#include "pineview.h"
#include "raminit.h"
#include <spd.h>
#include <string.h>

/* Debugging macros */
#if CONFIG(DEBUG_RAM_SETUP)
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
#define ONLY_DIMMA_IS_POPULATED(dimms, ch) (\
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2) && \
	!DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3)))
#define ONLY_DIMMB_IS_POPULATED(dimms, ch) (\
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3) && \
	!DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2)))
#define BOTH_DIMMS_ARE_POPULATED(dimms, ch) (\
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
#if CONFIG(DEBUG_RAM_SETUP)
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

/*
 * RAM Config:    DIMMB-DIMMA
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
	u8 i, chan;
	s->dt0mode = 0;
	FOR_EACH_DIMM(i) {
		if (i2c_eeprom_read(s->spd_map[i], 0, 64, s->dimms[i].spd_data) != 64)
			s->dimms[i].card_type = 0;

		s->dimms[i].card_type = s->dimms[i].spd_data[62] & 0x1f;
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
		PRINTK_DEBUG("  Config[CH%d] : %d\n", chan, s->dimm_config[chan]);
	}
}

#if CONFIG(DEBUG_RAM_SETUP)
static u32 fsb_reg_to_mhz(u32 speed)
{
	return (speed * 133) + 667;
}

static u32 ddr_reg_to_mhz(u32 speed)
{
	return (speed == 0) ? 667 : (speed == 1) ? 800 : 0;
}
#endif

// Return the position of the least significant set bit, 0-indexed.
// 0 does not have a lsb, so return -1 for error.
static int lsbpos(u8 val)
{
	for (int i = 0; i < 8; i++)
		if (val & (1 << i))
			return i;
	return -1;
}

// Return the position of the most significant set bit, 0-indexed.
// 0 does not have a msb, so return -1 for error.
static int msbpos(u8 val)
{
	for (int i = 7; i >= 0; i--)
		if (val & (1 << i))
			return i;
	return -1;
}

static void sdram_detect_smallest_params(struct sysinfo *s)
{
	static const u16 mult[6] = {
		3000, // 667
		2500, // 800
	};

	u8 i;
	u32 maxtras = 0;
	u32 maxtrp  = 0;
	u32 maxtrcd = 0;
	u32 maxtwr  = 0;
	u32 maxtrfc = 0;
	u32 maxtwtr = 0;
	u32 maxtrrd = 0;
	u32 maxtrtp = 0;

	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		maxtras = MAX(maxtras, (s->dimms[i].spd_data[30] * 1000));
		maxtrp  = MAX(maxtrp,  (s->dimms[i].spd_data[27] * 1000) >> 2);
		maxtrcd = MAX(maxtrcd, (s->dimms[i].spd_data[29] * 1000) >> 2);
		maxtwr  = MAX(maxtwr,  (s->dimms[i].spd_data[36] * 1000) >> 2);
		maxtrfc = MAX(maxtrfc, (s->dimms[i].spd_data[42] * 1000) +
				       (s->dimms[i].spd_data[40] & 0xf));
		maxtwtr = MAX(maxtwtr, (s->dimms[i].spd_data[37] * 1000) >> 2);
		maxtrrd = MAX(maxtrrd, (s->dimms[i].spd_data[28] * 1000) >> 2);
		maxtrtp = MAX(maxtrtp, (s->dimms[i].spd_data[38] * 1000) >> 2);
	}
	/*
	 * TODO: on DDR3 there might be some minimal required values for some
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
	u8 highcas   = 0;
	u8 lowcas    = 0;

	// Core frequency
	fsb = (pci_read_config8(HOST_BRIDGE, 0xe3) & 0x70) >> 4;
	if (fsb) {
		fsb = 5 - fsb;
	} else {
		fsb = FSB_CLOCK_800MHz;
	}

	// DDR frequency
	freq  = (pci_read_config8(HOST_BRIDGE, 0xe3) & 0x80) >> 7;
	freq |= (pci_read_config8(HOST_BRIDGE, 0xe4) & 0x03) << 1;
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

	// commoncas is nonzero, so these calls will not error
	u8 msbp = (u8)msbpos(commoncas);
	u8 lsbp = (u8)lsbpos(commoncas);

	// Start with fastest common CAS
	cas = 0;
	highcas = msbp;
	lowcas = MAX(lsbp, 5);

	while (cas == 0 && highcas >= lowcas) {
		FOR_EACH_POPULATED_DIMM(s->dimms, i) {
			switch (freq) {
			case MEM_CLOCK_800MHz:
				if ((s->dimms[i].spd_data[9]  > 0x25) ||
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
				if ((s->dimms[i].spd_data[9]  > 0x30) ||
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
		highcas = msbp;
		lowcas = lsbp;
		while (cas == 0 && highcas >= lowcas) {
			FOR_EACH_POPULATED_DIMM(s->dimms, i) {
				if ((s->dimms[i].spd_data[9]  > 0x30) ||
				    (s->dimms[i].spd_data[10] > 0x45)) {
					// CAS too fast, lower it
					highcas--;
				} else {
					cas = highcas;
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

	PRINTK_DEBUG("Drive Memory at %dMHz with CAS = %d clocks\n",
		     ddr_reg_to_mhz(s->selected_timings.mem_clock), s->selected_timings.CAS);

	// Set memory frequency
	if (s->boot_path == BOOT_PATH_RESET)
		return;

	MCHBAR32_OR(PMSTS, 1);

	reg32 = (MCHBAR32(CLKCFG) & ~0x70) | (1 << 10);
	if (s->selected_timings.mem_clock == MEM_CLOCK_800MHz) {
		reg8 = 3;
	} else {
		reg8 = 2;
	}
	reg32 |= reg8 << 4;
	MCHBAR32(CLKCFG) = reg32;

	s->selected_timings.mem_clock = ((MCHBAR32(CLKCFG) >> 4) & 0x7) - 2;
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
	reg32 = RCBA32(HPTC);
	reg32 &= ~0x03;
	reg32 |= (1 << 7);
	RCBA32(HPTC) = reg32;
	/* On NM10 this only works if read back */
	RCBA32(HPTC);
	HPET32(0x10) = HPET32(0x10) | 1;
}

static void sdram_clk_crossing(struct sysinfo *s)
{
	u8 ddr_freq, fsb_freq;
	static const u32 clkcross[2][2][4] = {
	{
		{0xFFFFFFFF, 0x05030305, 0x0000FFFF, 0x00000000}, /* FSB = 667, DDR = 667 */
		{0x1F1F1F1F, 0x2A1F1FA5, 0x00000000, 0x05000002}, /* FSB = 667, DDR = 800 */
	},
	{
		{0x1F1F1F1F, 0x0D07070B, 0x00000000, 0x00000000}, /* FSB = 800, DDR = 667 */
		{0xFFFFFFFF, 0x05030305, 0x0000FFFF, 0x00000000}, /* FSB = 800, DDR = 800 */
	},
	};

	ddr_freq = s->selected_timings.mem_clock;
	fsb_freq = s->selected_timings.fsb_clock;

	MCHBAR32(HMCCMP)   = clkcross[fsb_freq][ddr_freq][0];
	MCHBAR32(HMDCMP)   = clkcross[fsb_freq][ddr_freq][1];
	MCHBAR32(HMBYPCP)  = clkcross[fsb_freq][ddr_freq][2];
	MCHBAR32(HMCCPEXT) = 0;
	MCHBAR32(HMDCPEXT) = clkcross[fsb_freq][ddr_freq][3];

	MCHBAR32_OR(HMCCMC, 1 << 7);

	if ((fsb_freq == 0) && (ddr_freq == 1)) {
		MCHBAR8(CLKXSSH2MCBYPPHAS) = 0;
		MCHBAR32(CLKXSSH2MD)       = 0;
		MCHBAR32(CLKXSSH2MD + 4)   = 0;
	}

	static const u32 clkcross2[2][2][8] = {
	{
		{	// FSB = 667, DDR = 667
			0x00000000, 0x08010204, 0x00000000, 0x08010204,
			0x00000000, 0x00000000, 0x00000000, 0x04080102,
		},
		{	// FSB = 667, DDR = 800
			0x04080000, 0x10010002, 0x10000000, 0x20010208,
			0x00000000, 0x00000004, 0x02040000, 0x08100102,
		},
	},
	{
		{	// FSB = 800, DDR = 667
			0x10000000, 0x20010208, 0x04080000, 0x10010002,
			0x00000000, 0x00000000, 0x08000000, 0x10200204,
		},
		{	// FSB = 800, DDR = 800
			0x00000000, 0x08010204, 0x00000000, 0x08010204,
			0x00000000, 0x00000000, 0x00000000, 0x04080102,
		},
	},
	};

	MCHBAR32(CLKXSSH2MCBYP)       = clkcross2[fsb_freq][ddr_freq][0];
	MCHBAR32(CLKXSSH2MCRDQ)       = clkcross2[fsb_freq][ddr_freq][0];
	MCHBAR32(CLKXSSH2MCRDCST)     = clkcross2[fsb_freq][ddr_freq][0];
	MCHBAR32(CLKXSSH2MCBYP + 4)   = clkcross2[fsb_freq][ddr_freq][1];
	MCHBAR32(CLKXSSH2MCRDQ + 4)   = clkcross2[fsb_freq][ddr_freq][1];
	MCHBAR32(CLKXSSH2MCRDCST + 4) = clkcross2[fsb_freq][ddr_freq][1];
	MCHBAR32(CLKXSSMC2H)          = clkcross2[fsb_freq][ddr_freq][2];
	MCHBAR32(CLKXSSMC2H + 4)      = clkcross2[fsb_freq][ddr_freq][3];
	MCHBAR32(CLKXSSMC2HALT)       = clkcross2[fsb_freq][ddr_freq][4];
	MCHBAR32(CLKXSSMC2HALT + 4)   = clkcross2[fsb_freq][ddr_freq][5];
	MCHBAR32(CLKXSSH2X2MD)        = clkcross2[fsb_freq][ddr_freq][6];
	MCHBAR32(CLKXSSH2X2MD + 4)    = clkcross2[fsb_freq][ddr_freq][7];
}

static void sdram_clkmode(struct sysinfo *s)
{
	u8  ddr_freq;
	u16 mpll_ctl;

	MCHBAR16_AND(CSHRMISCCTL1, ~(1 << 8));
	MCHBAR8_AND(CSHRMISCCTL1,  ~0x3f);

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		ddr_freq = 0;
		mpll_ctl = 1;
	} else {
		ddr_freq = 1;
		mpll_ctl = (1 << 8) | (1 << 5);
	}
	if (s->boot_path != BOOT_PATH_RESET)
		MCHBAR16_AND_OR(MPLLCTL, ~(0x033f), mpll_ctl);

	MCHBAR32(C0GNT2LNCH1) = 0x58001117;
	MCHBAR32_OR(C0STATRDCTRL, 1 << 23);

	const u32 cas_to_reg[2][4] = {
		{0x00000000, 0x00030100, 0x0C240201, 0x00000000}, /* DDR = 667 */
		{0x00000000, 0x00030100, 0x0C240201, 0x10450302}  /* DDR = 800 */
	};

	MCHBAR32(C0GNT2LNCH2) = cas_to_reg[ddr_freq][s->selected_timings.CAS - 3];
}

static void sdram_timings(struct sysinfo *s)
{
	u8 i, j, ch, r, ta1, ta2, ta3, ta4, trp, bank, page, flag;
	u8 reg8, wl;
	u16 reg16;
	u32 reg32, reg2;

	static const u8 pagetab[2][2] = {
		{0x0e, 0x12},
		{0x10, 0x14},
	};

	/* Only consider DDR2 */
	wl   = s->selected_timings.CAS - 1;
	ta1  = ta2 = 6;
	ta3  = s->selected_timings.CAS;
	ta4  = 8;
	s->selected_timings.tRFC = (s->selected_timings.tRFC + 1) & 0xfe;
	trp  = 0;
	bank = 1;
	page = 0;

	MCHBAR8(C0LATCTRL) = ((wl - 3) << 4) | (s->selected_timings.CAS - 3);

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

	MCHBAR8_OR(C0PVCFG, 0x03);
	MCHBAR16(C0CYCTRKPCHG) = ((wl + 4 + s->selected_timings.tWR) << 6) |
				 ((2 + MAX(s->selected_timings.tRTP, 2)) << 2) | 1;

	reg32 = (bank << 21) | (s->selected_timings.tRRD << 17) |
		(s->selected_timings.tRP << 13) | ((s->selected_timings.tRP + trp) << 9) |
		 s->selected_timings.tRFC;

	if (bank == 0) {
		reg32 |= (pagetab[flag][page] << 22);
	}
	/* FIXME: Why not do a single dword write? */
	MCHBAR16(C0CYCTRKACT)     = (u16) (reg32);
	MCHBAR16(C0CYCTRKACT + 2) = (u16) (reg32 >> 16);

	/* FIXME: Only applies to DDR2 */
	reg16 = (MCHBAR16(C0CYCTRKACT + 2) & 0x0fc0) >> 6;
	MCHBAR16_AND_OR(SHCYCTRKCKEL, ~0x1f80, (reg16 << 7));

	reg16 = (s->selected_timings.tRCD << 12) | (4 << 8) | (ta2 << 4) | ta4;
	MCHBAR16(C0CYCTRKWR) = reg16;

	reg32 = (s->selected_timings.tRCD << 17) | ((wl + 4 + s->selected_timings.tWTR) << 12) |
		(ta3 << 8) | (4 << 4) | ta1;
	MCHBAR32(C0CYCTRKRD) = reg32;

	reg16 = ((s->selected_timings.tRP + trp) << 9) | s->selected_timings.tRFC;

	/* FIXME: Why not do a single word write? */
	MCHBAR8(C0CYCTRKREFR)     = (u8) (reg16);
	MCHBAR8(C0CYCTRKREFR + 1) = (u8) (reg16 >> 8);

	MCHBAR16_AND_OR(C0CKECTRL, ~0x03fe, 100 << 1);
	MCHBAR8_AND_OR(C0CYCTRKPCHG2, ~0x3f, s->selected_timings.tRAS);
	MCHBAR16(C0ARBCTRL) = 0x2310;
	MCHBAR8_AND_OR(C0ADDCSCTRL, ~0x1f, 1);

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
	reg16 = (u16)((((s->selected_timings.CAS + 7) * (reg32)) / reg2) << 8);
	MCHBAR16_AND_OR(C0STATRDCTRL, ~0x1f00, reg16);

	flag = 0;
	if (wl > 2) {
		flag = 1;
	}
	reg16 = (u8) (wl - 1 - flag);
	reg16 |= reg16 << 4;
	reg16 |= flag << 8;
	MCHBAR16_AND_OR(C0WRDATACTRL, ~0x01ff, reg16);

	MCHBAR16(C0RDQCTRL) = 0x1585;
	MCHBAR8_AND(C0PWLRCTRL, ~0x1f);

	/* rdmodwr_window[5..0] = CL+4+5  265[13..8] (264[21..16]) */
	MCHBAR16_AND_OR(C0PWLRCTRL, ~0x3f00, (s->selected_timings.CAS + 9) << 8);

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		reg16 = 0x0514;
		reg32 = 0x0a28;
	} else {
		reg16 = 0x0618;
		reg32 = 0x0c30;
	}
	MCHBAR32_AND_OR(C0REFRCTRL2, ~0x0fffff00, (0x3f << 22) | (reg32 << 8));

	/* FIXME: Is this weird access necessary? Reference code does it */
	MCHBAR8(C0REFRCTRL + 3) = 0;
	MCHBAR16_AND_OR(C0REFCTRL, 0xc000, reg16);

	/* NPUT Static Mode */
	MCHBAR8_OR(C0DYNRDCTRL, 1);

	MCHBAR32_AND_OR(C0STATRDCTRL, ~0x7f000000, 0xb << 25);
	i = s->selected_timings.mem_clock;
	j = s->selected_timings.fsb_clock;
	if (i > j) {
		MCHBAR32_OR(C0STATRDCTRL, 1 << 24);
	}

	MCHBAR8_AND(C0RDFIFOCTRL, ~0x3);
	MCHBAR16_AND_OR(C0WRDATACTRL, ~0x7c00, (wl + 10) << 10);
	MCHBAR32_AND_OR(C0CKECTRL, ~0x070e0000, (3 << 24) | (3 << 17));
	reg16 = 0x15 << 6;
	reg16 |= 0x1f;
	reg16 |= (0x6 << 12);
	MCHBAR16_AND_OR(C0REFRCTRL + 4, ~0x7fff, reg16);

	reg32 = (0x6 << 27) | (1 << 25);	/* FIXME: For DDR3, set BIT26 as well */
	MCHBAR32_AND_OR(C0REFRCTRL2, ~0x30000000, reg32 << 8);
	MCHBAR8_AND_OR(C0REFRCTRL + 3, ~0xfa, reg32 >> 24);
	MCHBAR8_AND(C0JEDEC, ~(1 << 7));
	MCHBAR8_AND(C0DYNRDCTRL, ~0x6);

	/* Note: This is a 64-bit register, [34..30] = 0b00110 is split across two writes */
	reg32 = ((6 & 3) << 30) | (4 << 25) | (1 << 20) | (8 << 15) | (6 << 10) | (4 << 5) | 1;
	MCHBAR32(C0WRWMFLSH) = reg32;
	MCHBAR16_AND_OR(C0WRWMFLSH + 4, ~0x1ff, (8 << 3) | (6 >> 2));
	MCHBAR16_OR(SHPENDREG, 0x1c00 | (0x1f << 5));

	/* FIXME: Why not do a single word write? */
	MCHBAR8_AND_OR(SHPAGECTRL,     ~0xff, 0x40);
	MCHBAR8_AND_OR(SHPAGECTRL + 1, ~0x07, 0x05);
	MCHBAR8_OR(SHCMPLWRCMD, 0x1f);

	reg8  = (3 << 6);
	reg8 |= (s->dt0mode << 4);
	reg8 |= 0x0c;
	MCHBAR8_AND_OR(SHBONUSREG, ~0xdf, reg8);
	MCHBAR8_AND(CSHRWRIOMLNS, ~0x02);
	MCHBAR8_AND_OR(C0MISCTM, ~0x07, 0x02);
	MCHBAR16_AND_OR(C0BYPCTRL, ~0x3fc, 4 << 2);

	/* [31..29] = 0b010 for kN = 2 (2N) */
	reg32 = (2 << 29) | (1 << 28) | (1 << 23);
	MCHBAR32_AND_OR(WRWMCONFIG, ~0xffb00000, reg32);

	reg8  = (u8) ((MCHBAR16(C0CYCTRKACT)     & 0xe000) >> 13);
	reg8 |= (u8) ((MCHBAR16(C0CYCTRKACT + 2) & 1) << 3);
	MCHBAR8_AND_OR(BYPACTSF, ~0xf0, reg8 << 4);

	reg8 = (u8) ((MCHBAR32(C0CYCTRKRD) & 0x000f0000) >> 17);
	MCHBAR8_AND_OR(BYPACTSF, ~0x0f, reg8);

	/* FIXME: Why not clear everything at once? */
	MCHBAR8_AND(BYPKNRULE, ~0xfc);
	MCHBAR8_AND(BYPKNRULE, ~0x03);
	MCHBAR8_AND(SHBONUSREG, ~0x03);
	MCHBAR8_OR(C0BYPCTRL, 1);
	MCHBAR16_OR(CSHRMISCCTL1, 1 << 9);

	for (i = 0; i < 8; i++) {
		/* FIXME: Hardcoded for DDR2 SO-DIMMs */
		MCHBAR32_AND_OR(C0DLLRCVCTLy(i), ~0x3f3f3f3f, 0x0c0c0c0c);
	}
	/* RDCS to RCVEN delay: Program coarse common to all bytelanes to default tCL + 1 */
	MCHBAR32_AND_OR(C0STATRDCTRL, ~0x000f0000, (s->selected_timings.CAS + 1) << 16);

	/* Program RCVEN delay with DLL-safe settings */
	for (i = 0; i < 8; i++) {
		MCHBAR8_AND(C0RXRCVyDLL(i), ~0x3f);
		MCHBAR16_AND(C0RCVMISCCTL2, (u16) ~(3 << (i * 2)));
		MCHBAR16_AND(C0RCVMISCCTL1, (u16) ~(3 << (i * 2)));
		MCHBAR16_AND(C0COARSEDLY0,  (u16) ~(3 << (i * 2)));
	}
	MCHBAR8_AND(C0DLLPIEN, ~1);	/* Power up receiver */
	MCHBAR8_OR(C0DLLPIEN,   2);	/* Enable RCVEN DLL */
	MCHBAR8_OR(C0DLLPIEN,   4);	/* Enable receiver DQS DLL */
	MCHBAR32_OR(C0COREBONUS, 0x000c0400);
	MCHBAR32_OR(C0CMDTX1, 1 << 31);
}

/* Program clkset0's register for Kcoarse, Tap, PI, DBEn and DBSel */
static void sdram_p_clkset0(const struct pllparam *pll, u8 f, u8 i)
{
	MCHBAR16_AND_OR(C0CKTX, ~0xc440,
			(pll->clkdelay[f][i] << 14) |
			(pll->dben[f][i] << 10) |
			(pll->dbsel[f][i] << 6));

	MCHBAR8_AND_OR(C0TXCK0DLL, ~0x3f, pll->pi[f][i]);
}

/* Program clkset1's register for Kcoarse, Tap, PI, DBEn and DBSel */
static void sdram_p_clkset1(const struct pllparam *pll, u8 f, u8 i)
{
	/* FIXME: This is actually a dword write! */
	MCHBAR16_AND_OR(C0CKTX, ~0x00030880,
			(pll->clkdelay[f][i] << 16) |
			(pll->dben[f][i] << 11) |
			(pll->dbsel[f][i] << 7));

	MCHBAR8_AND_OR(C0TXCK1DLL, ~0x3f, pll->pi[f][i]);
}

/* Program CMD0 and CMD1 registers for Kcoarse, Tap, PI, DBEn and DBSel */
static void sdram_p_cmd(const struct pllparam *pll, u8 f, u8 i)
{
	u8 reg8;
	/* Clock Group Index 3 */
	reg8 = pll->dbsel[f][i] << 5;
	reg8 |= pll->dben[f][i] << 6;
	MCHBAR8_AND_OR(C0CMDTX1, ~0x60, reg8);

	reg8 = pll->clkdelay[f][i] << 4;
	MCHBAR8_AND_OR(C0CMDTX2, ~0x30, reg8);

	reg8 = pll->pi[f][i];
	MCHBAR8_AND_OR(C0TXCMD0DLL, ~0x3f, reg8);
	MCHBAR8_AND_OR(C0TXCMD1DLL, ~0x3f, reg8);
}

/* Program CTRL registers for Kcoarse, Tap, PI, DBEn and DBSel */
static void sdram_p_ctrl(const struct pllparam *pll, u8 f, u8 i)
{
	u8 reg8;
	u32 reg32;

	/* CTRL0 and CTRL1 */
	reg32  = ((u32) pll->dbsel[f][i]) << 20;
	reg32 |= ((u32) pll->dben[f][i])  << 21;
	reg32 |= ((u32) pll->dbsel[f][i]) << 22;
	reg32 |= ((u32) pll->dben[f][i])  << 23;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 24;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 27;
	MCHBAR32_AND_OR(C0CTLTX2, ~0x01bf0000, reg32);

	reg8 = pll->pi[f][i];
	MCHBAR8_AND_OR(C0TXCTL0DLL, ~0x3f, reg8);
	MCHBAR8_AND_OR(C0TXCTL1DLL, ~0x3f, reg8);

	/* CTRL2 and CTRL3 */
	reg32  = ((u32) pll->dbsel[f][i]) << 12;
	reg32 |= ((u32) pll->dben[f][i])  << 13;
	reg32 |= ((u32) pll->dbsel[f][i]) << 8;
	reg32 |= ((u32) pll->dben[f][i])  << 9;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 14;
	reg32 |= ((u32) pll->clkdelay[f][i]) << 10;
	MCHBAR32_AND_OR(C0CMDTX2, ~0xff00, reg32);

	reg8 = pll->pi[f][i];
	MCHBAR8_AND_OR(C0TXCTL2DLL, ~0x3f, reg8);
	MCHBAR8_AND_OR(C0TXCTL3DLL, ~0x3f, reg8);
}

static void sdram_p_dqs(struct pllparam *pll, u8 f, u8 clk)
{
	u8 rank, dqs, reg8, j;
	u32 reg32;

	j     = clk - 40;
	reg8  = 0;
	reg32 = 0;
	rank  = j % 4;
	dqs   = j / 4;

	reg32 |= ((u32) pll->dben[f][clk])  << (dqs + 9);
	reg32 |= ((u32) pll->dbsel[f][clk]) << dqs;

	/* FIXME: Somehow, touching this changes the binary... */
	MCHBAR32(C0DQSRyTX1(rank)) = (MCHBAR32(0x5b4 + (rank * 4))
				   & ~((1 << (dqs + 9)) | (1 << dqs))) | reg32;

	reg32 = ((u32) pll->clkdelay[f][clk]) << ((dqs * 2) + 16);
	MCHBAR32_AND_OR(C0DQSDQRyTX3(rank), ~((1 << (dqs * 2 + 17)) | (1 << (dqs * 2 + 16))),
			reg32);

	reg8 = pll->pi[f][clk];
	MCHBAR8_AND_OR(C0TXDQS0R0DLL + j, ~0x3f, reg8);
}

static void sdram_p_dq(struct pllparam *pll, u8 f, u8 clk)
{
	u8 rank, dq, reg8, j;
	u32 reg32;

	j     = clk - 8;
	reg8  = 0;
	reg32 = 0;
	rank  = j % 4;
	dq    = j / 4;

	reg32 |= ((u32) pll->dben[f][clk])  << (dq + 9);
	reg32 |= ((u32) pll->dbsel[f][clk]) << dq;

	/* FIXME: Somehow, touching this changes the binary... */
	MCHBAR32(C0DQRyTX1(rank)) = (MCHBAR32(0x5a4 + rank * 4)
				  & ~((1 << (dq + 9)) | (1 << dq))) | reg32;

	reg32 = ((u32) pll->clkdelay[f][clk]) << (dq*2);
	MCHBAR32_AND_OR(C0DQSDQRyTX3(rank), ~((1 << (dq * 2 + 1)) | (1 << (dq * 2))), reg32);

	reg8 = pll->pi[f][clk];
	MCHBAR8_AND_OR(C0TXDQ0R0DLL + j, ~0x3f, reg8);
}

/* WDLL programming: Perform HPLL/MPLL calibration after write levelization */
static void sdram_calibratepll(struct sysinfo *s, u8 pidelay)
{
	struct pllparam pll = {
		.pi = {
		{	/* DDR = 667 */
			3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4,
			4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
			7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 3, 3,
		},
		{	/* DDR = 800 */
			53, 53, 10, 10,  5,  5,  5,  5, 27, 27, 27, 27,
			34, 34, 34, 34, 34, 34, 34, 34, 39, 39, 39, 39,
			47, 47, 47, 47, 44, 44, 44, 44, 47, 47, 47, 47,
			47, 47, 47, 47, 59, 59, 59, 59,  2,  2,  2,  2,
			 2,  2,  2,  2,  7,  7,  7,  7, 15, 15, 15, 15,
			12, 12, 12, 12, 15, 15, 15, 15, 15, 15, 15, 15,
		}},

		.dben = {
		{	/* DDR = 667 */
			0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		},
		{	/* DDR = 800 */
			1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
		}},

		.dbsel = {
		{	/* DDR = 667 */
			0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		},
		{	/* DDR = 800 */
			0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
		}},

		.clkdelay = {
		{	/* DDR = 667 */
			0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		},
		{	/* DDR = 800 */
			0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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

	/* Disable Dynamic DQS Slave Setting Per Rank */
	MCHBAR8_AND(CSHRDQSCMN, ~(1 << 7));
	MCHBAR16_AND_OR(CSHRPDCTL4, ~0x3fff, 0x1fff);

	sdram_p_clkset0(&pll, f, 0);
	sdram_p_clkset1(&pll, f, 1);
	sdram_p_cmd(&pll,     f, 2);
	sdram_p_ctrl(&pll,    f, 4);

	for (i = 0; i < 32; i++) {
		sdram_p_dqs(&pll, f, i + 40);
	}
	for (i = 0; i < 32; i++) {
		sdram_p_dq(&pll, f, i + 8);
	}
}

/* Perform HMC hardware calibration */
static void sdram_calibratehwpll(struct sysinfo *s)
{
	u8 reg8;

	s->async = 0;
	reg8 = 0;

	MCHBAR16_OR(CSHRPDCTL, 1 << 15);
	MCHBAR8_AND(CSHRPDCTL, ~(1 << 7));
	MCHBAR8_OR(CSHRPDCTL, 1 << 3);
	MCHBAR8_OR(CSHRPDCTL, 1 << 2);

	/* Start hardware HMC calibration */
	MCHBAR8_OR(CSHRPDCTL, 1 << 7);

	/* Busy-wait until calibration is done */
	while ((MCHBAR8(CSHRPDCTL) & (1 << 2)) == 0)
		;

	/* If hardware HMC calibration failed */
	reg8 = (MCHBAR8(CSHRPDCTL) & (1 << 3)) >> 3;
	if (reg8 != 0) {
		s->async = 1;
	}
}

static void sdram_dlltiming(struct sysinfo *s)
{
	u8 reg8, i;
	u16 reg16;
	u32 reg32;

	/* Configure the Master DLL */
	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		reg32 = 0x08014227;
	} else {
		reg32 = 0x00014221;
	}
	MCHBAR32_AND_OR(CSHRMSTRCTL1, ~0x0fffffff, reg32);
	MCHBAR32_OR(CSHRMSTRCTL1,    1 << 23);
	MCHBAR32_OR(CSHRMSTRCTL1,    1 << 15);
	MCHBAR32_AND(CSHRMSTRCTL1, ~(1 << 15));

	if (s->nodll) {
		/* Disable the Master DLLs by setting these bits, IN ORDER! */
		MCHBAR16_OR(CSHRMSTRCTL0, 1 << 0);
		MCHBAR16_OR(CSHRMSTRCTL0, 1 << 2);
		MCHBAR16_OR(CSHRMSTRCTL0, 1 << 4);
		MCHBAR16_OR(CSHRMSTRCTL0, 1 << 8);
		MCHBAR16_OR(CSHRMSTRCTL0, 1 << 10);
		MCHBAR16_OR(CSHRMSTRCTL0, 1 << 12);
		MCHBAR16_OR(CSHRMSTRCTL0, 1 << 14);
	} else {
		/* Enable the Master DLLs by clearing these bits, IN ORDER! */
		MCHBAR16_AND(CSHRMSTRCTL0, ~(1 <<  0));
		MCHBAR16_AND(CSHRMSTRCTL0, ~(1 <<  2));
		MCHBAR16_AND(CSHRMSTRCTL0, ~(1 <<  4));
		MCHBAR16_AND(CSHRMSTRCTL0, ~(1 <<  8));
		MCHBAR16_AND(CSHRMSTRCTL0, ~(1 << 10));
		MCHBAR16_AND(CSHRMSTRCTL0, ~(1 << 12));
		MCHBAR16_AND(CSHRMSTRCTL0, ~(1 << 14));
	}

	/* Initialize the Transmit DLL PI values in the following sequence. */
	if (s->nodll) {
		MCHBAR8_AND_OR(CREFPI, ~0x3f, 0x07);
	} else {
		MCHBAR8_AND(CREFPI, ~0x3f);
	}

	sdram_calibratepll(s, 0); // XXX check

	/* Enable all modular Slave DLL */
	MCHBAR16_OR(C0DLLPIEN, 1 << 11);
	MCHBAR16_OR(C0DLLPIEN, 1 << 12);

	for (i = 0; i < 8; i++) {
		MCHBAR16_OR(C0DLLPIEN, (1 << 10) >> i);
	}
	/* Enable DQ/DQS output */
	MCHBAR8_OR(C0SLVDLLOUTEN, 1);
	MCHBAR16(CSPDSLVWT) = 0x5005;
	MCHBAR16_AND_OR(CSHRPDCTL2, ~0x1f1f, 0x051a);
	MCHBAR16_AND_OR(CSHRPDCTL5, ~0xbf3f, 0x9010);

	if (s->nodll) {
		MCHBAR8_AND_OR(CSHRPDCTL3, ~0x7f, 0x6b);
	} else {
		MCHBAR8_AND_OR(CSHRPDCTL3, ~0x7f, 0x55);
		sdram_calibratehwpll(s);
	}
	/* Disable Dynamic Diff Amp */
	MCHBAR32_AND(C0STATRDCTRL, ~(1 << 22));

	/* Now, start initializing the transmit FIFO */
	MCHBAR8_AND(C0MISCCTL, ~0x02);

	/* Disable (gate) mdclk and mdclkb */
	MCHBAR8_OR(CSHWRIOBONUS, 0xc0);

	/* Select mdmclk */
	MCHBAR8_AND(CSHWRIOBONUS, ~(1 << 5));

	/* Ungate mdclk */
	MCHBAR8_AND_OR(CSHWRIOBONUS, ~0xc0, 1 << 6);
	MCHBAR8_AND_OR(CSHRFIFOCTL, ~0x3f, 0x1a);

	/* Enable the write pointer count */
	MCHBAR8_OR(CSHRFIFOCTL, 1);

	/* Set the DDR3 Reset Enable bit */
	MCHBAR8_OR(CSHRDDR3CTL, 1);

	/* Configure DQS-DQ Transmit */
	MCHBAR32(CSHRDQSTXPGM) = 0x00551803;

	reg8 = 0; /* Switch all clocks on anyway */

	/* Enable clock groups depending on rank population */
	MCHBAR32_AND_OR(C0CKTX, ~0x3f000000, reg8 << 24);

	/* Enable DDR command output buffers from core */
	MCHBAR8_AND(0x594, ~1);

	reg16 = 0;
	if (!rank_is_populated(s->dimms, 0, 0)) {
		reg16 |= (1 <<  8) | (1 << 4) | (1 << 0);
	}
	if (!rank_is_populated(s->dimms, 0, 1)) {
		reg16 |= (1 <<  9) | (1 << 5) | (1 << 1);
	}
	if (!rank_is_populated(s->dimms, 0, 2)) {
		reg16 |= (1 << 10) | (1 << 6) | (1 << 2);
	}
	if (!rank_is_populated(s->dimms, 0, 3)) {
		reg16 |= (1 << 11) | (1 << 7) | (1 << 3);
	}
	MCHBAR16_OR(C0CTLTX2, reg16);
}

/* Define a shorter name for these to make the lines fit in 96 characters */
#define TABLE static const

/* Loop over each RCOMP group, but skip group 1 because it does not exist */
#define FOR_EACH_RCOMP_GROUP(idx) for (idx = 0; idx < 7; idx++) if (idx != 1)

/* Define accessors for the RCOMP register banks */
#define C0RCOMPCTRLx(x)	(rcompctl[(x)] + 0x00)
#define C0RCOMPMULTx(x)	(rcompctl[(x)] + 0x04)
#define C0RCOMPOVRx(x)	(rcompctl[(x)] + 0x06)
#define C0RCOMPOSVx(x)	(rcompctl[(x)] + 0x0A)
#define C0SCOMPVREFx(x)	(rcompctl[(x)] + 0x0E)
#define C0SCOMPOVRx(x)	(rcompctl[(x)] + 0x10)
#define C0SCOMPOFFx(x)	(rcompctl[(x)] + 0x12)
#define C0DCOMPx(x)	(rcompctl[(x)] + 0x14)
#define C0SLEWBASEx(x)	(rcompctl[(x)] + 0x16)
#define C0SLEWPULUTx(x)	(rcompctl[(x)] + 0x18)
#define C0SLEWPDLUTx(x)	(rcompctl[(x)] + 0x1C)
#define C0DCOMPOVRx(x)	(rcompctl[(x)] + 0x20)
#define C0DCOMPOFFx(x)	(rcompctl[(x)] + 0x24)

/* FIXME: This only applies to DDR2 */
static void sdram_rcomp(struct sysinfo *s)
{
	u8  i, j, reg8, rcompp, rcompn, srup, srun;
	u16 reg16;
	u32 reg32, rcomp1, rcomp2;

	static const u8  rcompslew = 0x0a;
	static const u16 rcompctl[7] = {
		C0RCOMPCTRL0,
		0,		/* This register does not exist */
		C0RCOMPCTRL2,
		C0RCOMPCTRL3,
		C0RCOMPCTRL4,
		C0RCOMPCTRL5,
		C0RCOMPCTRL6,
	};

	/* RCOMP settings tables = { NC-NC,  x16SS,  x16DS, x16SS2, x16DS2,   x8DS,  x8DS2}; */
	TABLE u8  rcompupdate[7] = {     0,      0,      0,      1,      1,      0,      0};
	TABLE u8  rcompstr[7]    = {  0x66,   0x00,   0xaa,   0x55,   0x55,   0x77,   0x77};
	TABLE u16 rcompscomp[7]  = {0xa22a, 0x0000, 0xe22e, 0xe22e, 0xe22e, 0xa22a, 0xa22a};
	TABLE u8  rcompdelay[7]  = {     1,      0,      0,      0,      0,      1,      1};
	TABLE u16 rcompf[7]      = {0x1114, 0x0000, 0x0505, 0x0909, 0x0909, 0x0a0a, 0x0a0a};
	TABLE u8  rcompstr2[7]   = {  0x00,   0x55,   0x55,   0xaa,   0xaa,   0x55,   0xaa};
	TABLE u16 rcompscomp2[7] = {0x0000, 0xe22e, 0xe22e, 0xe22e, 0x8228, 0xe22e, 0x8228};
	TABLE u8  rcompdelay2[7] = {     0,      0,      0,      0,      2,      0,      2};

	TABLE u8 rcomplut[64][12] = {
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{ 9,  9, 11, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{10,  9, 12, 11,  2,  2,  5,  5,  6,  6,  5,  5},
		{10,  9, 12, 11,  2,  2,  6,  5,  7,  6,  6,  5},
		{10, 10, 12, 12,  2,  2,  6,  5,  7,  6,  6,  5},
		{10, 10, 12, 12,  2,  2,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  2,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  2,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  2,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  3,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  3,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  3,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  3,  6,  6,  7,  7,  6,  6},
		{10, 10, 12, 12,  3,  3,  6,  6,  7,  7,  6,  6},
		{11, 10, 12, 12,  3,  3,  6,  6,  7,  7,  6,  6},
		{11, 10, 14, 13,  3,  3,  6,  6,  7,  7,  6,  6},
		{12, 10, 14, 13,  3,  3,  6,  6,  7,  7,  6,  6},
		{12, 12, 14, 13,  3,  3,  7,  6,  7,  7,  7,  6},
		{13, 12, 16, 15,  3,  3,  7,  6,  8,  7,  7,  6},
		{13, 14, 16, 15,  4,  3,  7,  7,  8,  8,  7,  7},
		{14, 14, 16, 17,  4,  3,  7,  7,  8,  8,  7,  7},
		{14, 16, 18, 17,  4,  4,  8,  7,  8,  8,  8,  7},
		{15, 16, 18, 19,  4,  4,  8,  7,  9,  8,  8,  7},
		{15, 18, 18, 19,  4,  4,  8,  8,  9,  9,  8,  8},
		{16, 18, 20, 21,  4,  4,  8,  8,  9,  9,  8,  8},
		{16, 19, 20, 21,  5,  4,  9,  8, 10,  9,  9,  8},
		{16, 19, 20, 23,  5,  5,  9,  9, 10, 10,  9,  9},
		{17, 19, 22, 23,  5,  5,  9,  9, 10, 10,  9,  9},
		{17, 20, 22, 25,  5,  5,  9,  9, 10, 10,  9,  9},
		{17, 20, 22, 25,  5,  5,  9,  9, 10, 10,  9,  9},
		{18, 20, 22, 25,  5,  5,  9,  9, 10, 10,  9,  9},
		{18, 21, 24, 25,  5,  5,  9,  9, 11, 10,  9,  9},
		{19, 21, 24, 27,  5,  5,  9,  9, 11, 11,  9,  9},
		{19, 22, 24, 27,  5,  5, 10,  9, 11, 11, 10,  9},
		{20, 22, 24, 27,  6,  5, 10, 10, 11, 11, 10, 10},
		{20, 23, 26, 27,  6,  6, 10, 10, 12, 12, 10, 10},
		{20, 23, 26, 29,  6,  6, 10, 10, 12, 12, 10, 10},
		{21, 24, 26, 29,  6,  6, 10, 10, 12, 12, 10, 10},
		{21, 24, 26, 29,  6,  6, 11, 10, 12, 13, 11, 10},
		{22, 25, 28, 29,  6,  6, 11, 11, 13, 13, 11, 11},
		{22, 25, 28, 31,  6,  6, 11, 11, 13, 13, 11, 11},
		{22, 26, 28, 31,  6,  6, 11, 11, 13, 14, 11, 11},
		{23, 26, 30, 31,  7,  6, 12, 11, 14, 14, 12, 11},
		{23, 27, 30, 33,  7,  7, 12, 12, 14, 14, 12, 12},
		{23, 27, 30, 33,  7,  7, 12, 12, 14, 15, 12, 12},
		{24, 28, 32, 33,  7,  7, 12, 12, 15, 15, 12, 12},
		{24, 28, 32, 33,  7,  7, 12, 12, 15, 16, 12, 12},
		{24, 29, 32, 35,  7,  7, 12, 12, 15, 16, 12, 12},
		{25, 29, 32, 35,  7,  7, 12, 12, 15, 17, 12, 12},
		{25, 30, 32, 35,  7,  7, 12, 12, 15, 17, 12, 12},
		{25, 30, 32, 35,  7,  7, 12, 12, 15, 17, 12, 12},
	};

	srup = 0;
	srun = 0;

	if (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) {
		rcomp1 = 0x00050431;
	} else {
		rcomp1 = 0x00050542;
	}
	if (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz) {
		rcomp2 = 0x14C42827;
	} else {
		rcomp2 = 0x19042827;
	}

	FOR_EACH_RCOMP_GROUP(i) {
		reg8 = rcompupdate[i];
		MCHBAR8_AND_OR(C0RCOMPCTRLx(i), ~1, reg8);
		MCHBAR8_AND(C0RCOMPCTRLx(i), ~2);

		reg16 = rcompslew;
		MCHBAR16_AND_OR(C0RCOMPCTRLx(i), ~0xf000, reg16 << 12);

		MCHBAR8(C0RCOMPMULTx(i)) = rcompstr[i];
		MCHBAR16(C0SCOMPVREFx(i)) = rcompscomp[i];
		MCHBAR8_AND_OR(C0DCOMPx(i), ~0x03, rcompdelay[i]);
		if (i == 2) {
			/* FIXME: Why are we rewriting this? */
			MCHBAR16_AND_OR(C0RCOMPCTRLx(i), ~0xf000, reg16 << 12);

			MCHBAR8(C0RCOMPMULTx(i)) = rcompstr2[s->dimm_config[0]];
			MCHBAR16(C0SCOMPVREFx(i)) = rcompscomp2[s->dimm_config[0]];
			MCHBAR8_AND_OR(C0DCOMPx(i), ~0x03, rcompdelay2[s->dimm_config[0]]);
		}

		MCHBAR16_AND(C0SLEWBASEx(i), ~0x7f7f);

		/* FIXME: Why not do a single dword write? */
		MCHBAR16_AND(C0SLEWPULUTx(i),     ~0x3f3f);
		MCHBAR16_AND(C0SLEWPULUTx(i) + 2, ~0x3f3f);

		/* FIXME: Why not do a single dword write? */
		MCHBAR16_AND(C0SLEWPDLUTx(i),     ~0x3f3f);
		MCHBAR16_AND(C0SLEWPDLUTx(i) + 2, ~0x3f3f);
	}

	/* FIXME: Hardcoded */
	MCHBAR8_AND_OR(C0ODTRECORDX,    ~0x3f, 0x36);
	MCHBAR8_AND_OR(C0DQSODTRECORDX, ~0x3f, 0x36);

	FOR_EACH_RCOMP_GROUP(i) {
		MCHBAR8_AND(C0RCOMPCTRLx(i),      ~0x60);
		MCHBAR16_AND(C0RCOMPCTRLx(i) + 2, ~0x0706);
		MCHBAR16_AND(C0RCOMPOSVx(i),      ~0x7f7f);
		MCHBAR16_AND(C0SCOMPOFFx(i),      ~0x3f3f);
		MCHBAR16_AND(C0DCOMPOFFx(i),      ~0x1f1f);
		MCHBAR8_AND(C0DCOMPOFFx(i) + 2,   ~0x1f);
	}

	MCHBAR16_AND(C0ODTRECORDX,     ~0xffc0);
	MCHBAR16_AND(C0ODTRECORDX + 2, ~0x000f);

	/* FIXME: Why not do a single dword write? */
	MCHBAR16_AND(C0DQSODTRECORDX,     ~0xffc0);
	MCHBAR16_AND(C0DQSODTRECORDX + 2, ~0x000f);

	FOR_EACH_RCOMP_GROUP(i) {
		MCHBAR16(C0SCOMPOVRx(i)) = rcompf[i];

		/* FIXME: Why not do a single dword write? */
		MCHBAR16(C0DCOMPOVRx(i))     = 0x1219;
		MCHBAR16(C0DCOMPOVRx(i) + 2) = 0x000C;
	}

	MCHBAR32_AND_OR(DCMEASBUFOVR, ~0x001f1f1f, 0x000c1219);

	/* FIXME: Why not do a single word write? */
	MCHBAR16_AND_OR(XCOMPSDR0BNS, ~0x1f00, 0x1200);
	MCHBAR8_AND_OR(XCOMPSDR0BNS, ~0x1f, 0x12);

	MCHBAR32(COMPCTRL3) = 0x007C9007;
	MCHBAR32(OFREQDELSEL) = rcomp1;
	MCHBAR16(XCOMPCMNBNS) = 0x1f7f;
	MCHBAR32(COMPCTRL2) = rcomp2;
	MCHBAR16_AND_OR(XCOMPDFCTRL, ~0x0f, 1);
	MCHBAR16(ZQCALCTRL) = 0x0134;
	MCHBAR32(COMPCTRL1) = 0x4C293600;

	/* FIXME: wtf did these MRC guys smoke */
	MCHBAR8_AND_OR(COMPCTRL1 + 3, ~0x44, (1 << 6) | (1 << 2));
	MCHBAR16_AND(XCOMPSDR0BNS, ~(1 << 13));
	MCHBAR8_AND(XCOMPSDR0BNS,  ~(1 <<  5));

	FOR_EACH_RCOMP_GROUP(i) {
		/* FIXME: This should be an _AND_OR */
		MCHBAR8(C0RCOMPCTRLx(i) + 2) = MCHBAR8(C0RCOMPCTRLx(i)) & ~0x71;
	}

	if ((MCHBAR32(COMPCTRL1) & (1 << 30)) == 0) {
		/* Start COMP */
		MCHBAR8_OR(COMPCTRL1, 1);

		/* Wait until COMP is done */
		while ((MCHBAR8(COMPCTRL1) & 1) != 0)
			;

		reg32 = MCHBAR32(XCOMP);
		rcompp = (u8) ((reg32 & ~(1 << 31)) >> 24);
		rcompn = (u8) ((reg32 & ~(0xff800000)) >> 16);

		FOR_EACH_RCOMP_GROUP(i) {
			srup = (MCHBAR8(C0RCOMPCTRLx(i) + 1) & 0xc0) >> 6;
			srun = (MCHBAR8(C0RCOMPCTRLx(i) + 1) & 0x30) >> 4;

			/* FIXME: Why not do a single word write? */
			reg16 = (u16)(rcompp - (1 << (srup + 1))) << 8;
			MCHBAR16_AND_OR(C0SLEWBASEx(i), ~0x7f00, reg16);

			reg16 = (u16)(rcompn - (1 << (srun + 1)));
			MCHBAR8_AND_OR(C0SLEWBASEx(i), ~0x7f, (u8)reg16);
		}

		reg8 = rcompp - (1 << (srup + 1));
		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			MCHBAR8_AND_OR(C0SLEWPULUTx(0) + i, ~0x3f, rcomplut[j][0]);
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			if (s->dimm_config[0] < 3 || s->dimm_config[0] == 5) {
				MCHBAR8_AND_OR(C0SLEWPULUTx(2) + i, ~0x3f, rcomplut[j][10]);
			}
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			MCHBAR8_AND_OR(C0SLEWPULUTx(3) + i, ~0x3f, rcomplut[j][6]);
			MCHBAR8_AND_OR(C0SLEWPULUTx(4) + i, ~0x3f, rcomplut[j][6]);
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srup)) {
			MCHBAR8_AND_OR(C0SLEWPULUTx(5) + i, ~0x3f, rcomplut[j][8]);
			MCHBAR8_AND_OR(C0SLEWPULUTx(6) + i, ~0x3f, rcomplut[j][8]);
		}

		reg8 = rcompn - (1 << (srun + 1));
		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			MCHBAR8_AND_OR(C0SLEWPDLUTx(0) + i, ~0x3f, rcomplut[j][1]);
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			if (s->dimm_config[0] < 3 || s->dimm_config[0] == 5) {
				MCHBAR8_AND_OR(C0SLEWPDLUTx(2) + i, ~0x3f, rcomplut[j][11]);
			}
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			MCHBAR8_AND_OR(C0SLEWPDLUTx(3) + i, ~0x3f, rcomplut[j][7]);
			MCHBAR8_AND_OR(C0SLEWPDLUTx(4) + i, ~0x3f, rcomplut[j][7]);
		}

		for (i = 0, j = reg8; i < 4; i++, j += (1 << srun)) {
			MCHBAR8_AND_OR(C0SLEWPDLUTx(5) + i, ~0x3f, rcomplut[j][9]);
			MCHBAR8_AND_OR(C0SLEWPDLUTx(6) + i, ~0x3f, rcomplut[j][9]);
		}
	}
	MCHBAR8_OR(COMPCTRL1, 1);
}

/* FIXME: The ODT tables are for DDR2 only! */
static void sdram_odt(struct sysinfo *s)
{
	u8 rankindex = 0;

	static const u16 odt_rankctrl[16] = {
	/*	 NC_NC,  1R_NC,     NV,  2R_NC,  NC_1R,  1R_1R,     NV,  2R_1R, */
		0x0000, 0x0000, 0x0000, 0x0000, 0x0044, 0x1111, 0x0000, 0x1111,
	/*	    NV,     NV,     NV,     NV,  NC_2R,  1R_2R,     NV,  2R_2R, */
		0x0000, 0x0000, 0x0000, 0x0000, 0x0044, 0x1111, 0x0000, 0x1111,
	};
	static const u16 odt_matrix[16] = {
	/*	 NC_NC,  1R_NC,     NV,  2R_NC,  NC_1R,  1R_1R,     NV,  2R_1R, */
		0x0000, 0x0011, 0x0000, 0x0011, 0x0000, 0x4444, 0x0000, 0x4444,
	/*	    NV,     NV,     NV,     NV,  NC_2R,  1R_2R,     NV,  2R_2R, */
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4444, 0x0000, 0x4444,
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

	/* Program the ODT Matrix */
	MCHBAR16(C0ODT) = odt_matrix[rankindex];

	/* Program the ODT Rank Control */
	MCHBAR16(C0ODTRKCTRL) = odt_rankctrl[rankindex];
}

static void sdram_mmap(struct sysinfo *s)
{
	TABLE u32 w260[7] = {0, 0x400001,  0xc00001, 0x500000,  0xf00000,  0xc00001,  0xf00000};
	TABLE u32 w208[7] = {0,  0x10000, 0x1010000,  0x10001, 0x1010101, 0x1010000, 0x1010101};
	TABLE u32 w200[7] = {0,        0,         0,  0x20002,   0x40002,         0,   0x40002};
	TABLE u32 w204[7] = {0,  0x20002,   0x40002,  0x40004,   0x80006,   0x40002,   0x80006};

	TABLE u16 tolud[7]  = {2048, 2048, 4096, 4096, 8192, 4096, 8192};
	TABLE u16 tom[7]    = {   2,    2,    4,    4,    8,    4,    8};
	TABLE u16 touud[7]  = { 128,  128,  256,  256,  512,  256,  512};
	TABLE u32 gbsm[7]   = {1 << 27, 1 << 27, 1 << 28, 1 << 27, 1 << 29, 1 << 28, 1 << 29};
	TABLE u32 bgsm[7]   = {1 << 27, 1 << 27, 1 << 28, 1 << 27, 1 << 29, 1 << 28, 1 << 29};
	TABLE u32 tsegmb[7] = {1 << 27, 1 << 27, 1 << 28, 1 << 27, 1 << 29, 1 << 28, 1 << 29};

	if ((s->dimm_config[0] < 3) && rank_is_populated(s->dimms, 0, 0)) {
		if (s->dimms[0].sides > 1) {
			// 2R/NC
			MCHBAR32_AND_OR(C0CKECTRL, ~1, 0x300001);
			MCHBAR32(C0DRA01) = 0x00000101;
			MCHBAR32(C0DRB0)  = 0x00040002;
			MCHBAR32(C0DRB2)  = w204[s->dimm_config[0]];
		} else {
			// 1R/NC
			MCHBAR32_AND_OR(C0CKECTRL, ~1, 0x100001);
			MCHBAR32(C0DRA01) = 0x00000001;
			MCHBAR32(C0DRB0)  = 0x00020002;
			MCHBAR32(C0DRB2)  = w204[s->dimm_config[0]];
		}
	} else if ((s->dimm_config[0] == 5) && rank_is_populated(s->dimms, 0, 0)) {
		MCHBAR32_AND_OR(C0CKECTRL, ~1, 0x300001);
		MCHBAR32(C0DRA01) = 0x00000101;
		MCHBAR32(C0DRB0)  = 0x00040002;
		MCHBAR32(C0DRB2)  = 0x00040004;
	} else {
		MCHBAR32_AND_OR(C0CKECTRL, ~1, w260[s->dimm_config[0]]);
		MCHBAR32(C0DRA01) = w208[s->dimm_config[0]];
		MCHBAR32(C0DRB0)  = w200[s->dimm_config[0]];
		MCHBAR32(C0DRB2)  = w204[s->dimm_config[0]];
	}
	pci_write_config16(HOST_BRIDGE, 0xb0,  tolud[s->dimm_config[0]]);
	pci_write_config16(HOST_BRIDGE, 0xa0,    tom[s->dimm_config[0]]);
	pci_write_config16(HOST_BRIDGE, 0xa2,  touud[s->dimm_config[0]]);
	pci_write_config32(HOST_BRIDGE, 0xa4,   gbsm[s->dimm_config[0]]);
	pci_write_config32(HOST_BRIDGE, 0xa8,   bgsm[s->dimm_config[0]]);
	pci_write_config32(HOST_BRIDGE, 0xac, tsegmb[s->dimm_config[0]]);
}

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

static u8 sdram_checkrcompoverride(void)
{
	u32 xcomp;
	u8 aa, bb, a, b, c, d;

	xcomp = MCHBAR32(XCOMP);
	a = (u8)((xcomp & 0x7f000000) >> 24);
	b = (u8)((xcomp & 0x007f0000) >> 16);
	c = (u8)((xcomp & 0x00003f00) >>  8);
	d = (u8)((xcomp & 0x0000003f) >>  0);

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
	if ((aa > 18) || (bb > 7) || (a <= 5) || (b <= 5) || (c <= 5) || (d <= 5) ||
			(a >= 0x7a) || (b >= 0x7a) || (c >= 0x3a) || (d >= 0x3a)) {
		MCHBAR32(RCMEASBUFXOVR) = 0x9718a729;
		return 1;
	}
	return 0;
}

static void sdram_rcompupdate(struct sysinfo *s)
{
	u8 i, ok;
	u32 reg32a, reg32b;

	ok = 0;
	MCHBAR8_AND(XCOMPDFCTRL, ~(1 << 3));
	MCHBAR8_AND(COMPCTRL1, ~(1 << 7));
	for (i = 0; i < 3; i++) {
		MCHBAR8_OR(COMPCTRL1, 1);
		hpet_udelay(1000);
		while ((MCHBAR8(COMPCTRL1) & 1) != 0)
			;
		ok |= sdram_checkrcompoverride();
	}
	if (!ok) {
		reg32a = MCHBAR32(XCOMP);
		reg32b = ((reg32a >> 16) & 0x0000ffff);
		reg32a = ((reg32a << 16) & 0xffff0000) | reg32b;
		reg32a |= (1 << 31) | (1 << 15);
		MCHBAR32(RCMEASBUFXOVR) = reg32a;
	}
	MCHBAR8_OR(COMPCTRL1, 1);
	hpet_udelay(1000);
	while ((MCHBAR8(COMPCTRL1) & 1) != 0)
		;
}

static void __attribute__((noinline))
sdram_jedec(struct sysinfo *s, u8 rank, u8 jmode, u16 jval)
{
	u32 reg32;

	reg32 = jval << 3;
	reg32 |= rank * (1 << 27);
	MCHBAR8_AND_OR(C0JEDEC, ~0x3e, jmode);
	read32((void *)reg32);
	barrier();
	hpet_udelay(1); // 1us
}

static void sdram_zqcl(struct sysinfo *s)
{
	if (s->boot_path == BOOT_PATH_RESUME) {
		MCHBAR32_OR(C0CKECTRL, 1 << 27);
		MCHBAR8_AND_OR(C0JEDEC, ~0x0e, NORMAL_OP_CMD);
		MCHBAR8_AND(C0JEDEC, ~0x30);
		MCHBAR32_AND_OR(C0REFRCTRL2, ~(3 << 30), 3 << 30);
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
		((s->selected_timings.tWR - 1) << 9) | (1 << 3) | (1 << 1) | 3;

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
	reg32 |= (4 << 13);
	reg32 |= (6 <<  8);
	MCHBAR32_AND_OR(C0DYNRDCTRL, ~0x3ff00, reg32);
	MCHBAR8_AND(C0DYNRDCTRL, ~(1 << 7));
	MCHBAR8_OR(C0REFRCTRL + 3, 1);
	if (s->boot_path != BOOT_PATH_RESUME) {
		MCHBAR8_AND_OR(C0JEDEC, ~0x0e, NORMAL_OP_CMD);
		MCHBAR8_AND(C0JEDEC, ~0x30);
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
	if (reset)
		full_reset();
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
		dra = dratab
			[s->dimms[i].banks]
			[s->dimms[i].width]
			[s->dimms[i].cols - 9]
			[s->dimms[i].rows - 12];

		if (s->dimms[i].banks == 1) {
			dra |= (1 << 7);
		}
		reg32 |= (dra << (r * 8));
	}
	MCHBAR32(C0DRA01) = reg32;
	c0dra = reg32;
	PRINTK_DEBUG("C0DRA = 0x%08x\n", c0dra);

	reg32 = 0;
	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		reg32 |= (1 << r);
	}
	reg8 = (u8)(reg32 << 4) & 0xf0;
	MCHBAR8_AND_OR(C0CKECTRL + 2, ~0xf0, reg8);

	if (ONLY_DIMMA_IS_POPULATED(s->dimms, 0) || ONLY_DIMMB_IS_POPULATED(s->dimms, 0)) {
		MCHBAR8_OR(C0CKECTRL, 1);
	}

	addr = C0DRB0;
	c0drb = 0;
	FOR_EACH_RANK(ch, r) {
		if (rank_is_populated(s->dimms, ch, r)) {
			ind = (c0dra >> (8 * r)) & 0x7f;
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
	u8 dqsmatches = 1;
	while (count--) {
		MCHBAR8_AND(C0RSTCTL, ~2);
		hpet_udelay(1);
		MCHBAR8_OR(C0RSTCTL, 2);
		hpet_udelay(1);
		barrier();
		read32((void *)strobeaddr);
		barrier();
		hpet_udelay(1);

		if (((MCHBAR8(dqshighaddr) & 0x40) >> 6) != highlow) {
			dqsmatches = 0;
		}
	}

	return dqsmatches;
}

static void rcvenclock(u8 *coarse, u8 *medium, u8 lane)
{
	if (*medium < 3) {
		(*medium)++;
		MCHBAR16_AND_OR(C0RCVMISCCTL2, (u16)~(3 << (lane * 2)), *medium << (lane * 2));
	} else {
		*medium = 0;
		(*coarse)++;
		MCHBAR32_AND_OR(C0STATRDCTRL, ~0x000f0000, *coarse << 16);
		MCHBAR16_AND_OR(C0RCVMISCCTL2, (u16)(~3 << (lane * 2)), *medium << (lane * 2));
	}
}

static void sdram_rcven(struct sysinfo *s)
{
	u8 coarse, savecoarse;
	u8 medium, savemedium;
	u8 pi, savepi;
	u8 lane;
	u8 lanecoarse[8] = {0};
	u8 minlanecoarse = 0xff;
	u8 offset;
	u8 maxlane = 8;
	/* Since dra/drb is already set up we know that at address 0x00000000
	   we will always find the first available rank */
	u32 strobeaddr = 0;
	u32 dqshighaddr;

	MCHBAR8_AND(C0RSTCTL, ~0x0c);
	MCHBAR8_AND(CMNDQFIFORST, ~0x80);

	PRINTK_DEBUG("rcven 0\n");
	for (lane = 0; lane < maxlane; lane++) {
		PRINTK_DEBUG("rcven lane %d\n", lane);
// trylaneagain:
		dqshighaddr = C0MISCCTLy(lane);

		coarse = s->selected_timings.CAS + 1;
		pi = 0;
		medium = 0;

		MCHBAR32_AND_OR(C0STATRDCTRL, ~0x000f0000, coarse << 16);
		MCHBAR16_AND_OR(C0RCVMISCCTL2, (u16)~(3 << (lane * 2)), medium << (lane * 2));

		MCHBAR8_AND(C0RXRCVyDLL(lane), ~0x3f);

		savecoarse = coarse;
		savemedium = medium;
		savepi = pi;

		PRINTK_DEBUG("rcven 0.1\n");

		// XXX comment out
		// MCHBAR16_AND_OR(C0RCVMISCCTL1, (u16)~3 << (lane * 2), 1 << (lane * 2));

		while (sampledqs(dqshighaddr, strobeaddr, 0, 3) == 0) {
			// printk(BIOS_DEBUG, "coarse=%d medium=%d\n", coarse, medium);
			rcvenclock(&coarse, &medium, lane);
			if (coarse > 0xf) {
				PRINTK_DEBUG("Error: coarse > 0xf\n");
				// goto trylaneagain;
				break;
			}
		}
		PRINTK_DEBUG("rcven 0.2\n");

		savecoarse = coarse;
		savemedium = medium;
		rcvenclock(&coarse, &medium, lane);

		while (sampledqs(dqshighaddr, strobeaddr, 1, 3) == 0) {
			savecoarse = coarse;
			savemedium = medium;
			rcvenclock(&coarse, &medium, lane);
			if (coarse > 0xf) {
				PRINTK_DEBUG("Error: coarse > 0xf\n");
				//goto trylaneagain;
				break;
			}
		}

		PRINTK_DEBUG("rcven 0.3\n");
		coarse = savecoarse;
		medium = savemedium;
		MCHBAR32_AND_OR(C0STATRDCTRL, ~0x000f0000, coarse << 16);
		MCHBAR16_AND_OR(C0RCVMISCCTL2, (u16)~(0x3 << lane * 2), medium << (lane * 2));

		while (sampledqs(dqshighaddr, strobeaddr, 1, 3) == 0) {
			savepi = pi;
			pi++;
			if (pi > s->maxpi) {
				// if (s->nodll) {
					pi = savepi = s->maxpi;
					break;
				// }
			}
			MCHBAR8_AND_OR(C0RXRCVyDLL(lane), ~0x3f, pi << s->pioffset);
		}
		PRINTK_DEBUG("rcven 0.4\n");

		pi = savepi;
		MCHBAR8_AND_OR(C0RXRCVyDLL(lane), ~0x3f, pi << s->pioffset);
		rcvenclock(&coarse, &medium, lane);

		if (sampledqs(dqshighaddr, strobeaddr, 1, 1) == 0) {
			PRINTK_DEBUG("Error: DQS not high\n");
			// goto trylaneagain;
		}
		PRINTK_DEBUG("rcven 0.5\n");
		while (sampledqs(dqshighaddr, strobeaddr, 0, 3) == 0) {
			coarse--;
			MCHBAR32_AND_OR(C0STATRDCTRL, ~0x000f0000, coarse << 16);
			if (coarse == 0) {
				PRINTK_DEBUG("Error: DQS did not hit 0\n");
				break;
			}
		}

		PRINTK_DEBUG("rcven 0.6\n");
		rcvenclock(&coarse, &medium, lane);
		s->pi[lane] = pi;
		lanecoarse[lane] = coarse;
	}

	PRINTK_DEBUG("rcven 1\n");

	lane = maxlane;
	do {
		lane--;
		if (minlanecoarse > lanecoarse[lane]) {
			minlanecoarse = lanecoarse[lane];
		}
	} while (lane != 0);

	lane = maxlane;
	do {
		lane--;
		offset = lanecoarse[lane] - minlanecoarse;
		MCHBAR16_AND_OR(C0COARSEDLY0, (u16)(~(3 << (lane * 2))), offset << (lane * 2));
	} while (lane != 0);

	MCHBAR32_AND_OR(C0STATRDCTRL, ~0x000f0000, minlanecoarse << 16);

	s->coarsectrl   = minlanecoarse;
	s->coarsedelay  = MCHBAR16(C0COARSEDLY0);
	s->mediumphase  = MCHBAR16(C0RCVMISCCTL2);
	s->readptrdelay = MCHBAR16(C0RCVMISCCTL1);

	PRINTK_DEBUG("rcven 2\n");
	MCHBAR8_AND(C0RSTCTL, ~0x0e);
	MCHBAR8_OR(C0RSTCTL,   0x02);
	MCHBAR8_OR(C0RSTCTL,   0x04);
	MCHBAR8_OR(C0RSTCTL,   0x08);

	MCHBAR8_OR(CMNDQFIFORST,   0x80);
	MCHBAR8_AND(CMNDQFIFORST, ~0x80);
	MCHBAR8_OR(CMNDQFIFORST,   0x80);
	PRINTK_DEBUG("rcven 3\n");
}

/* NOTE: Unless otherwise specified, the values are expressed in MiB */
static void sdram_mmap_regs(struct sysinfo *s)
{
	bool reclaim;
	u32 mmiosize, tom, tolud, touud, reclaimbase, reclaimlimit;
	u32 gfxbase, gfxsize, gttbase, gttsize, tsegbase, tsegsize;
	u16 ggc;
	u16 ggc_to_uma[10] = {0, 1, 4, 8, 16, 32, 48, 64, 128, 256};
	u8  ggc_to_gtt[4]  = {0, 1, 0, 0};

	reclaimbase  = 0;
	reclaimlimit = 0;

	ggc = pci_read_config16(HOST_BRIDGE, GGC);
	printk(BIOS_DEBUG, "GGC = 0x%04x\n", ggc);

	gfxsize = ggc_to_uma[(ggc & 0x00f0) >> 4];

	gttsize = ggc_to_gtt[(ggc & 0x0300) >> 8];

	tom = s->channel_capacity[0];

	/* With GTT always being 1M, TSEG 1M is the only setting that can
	   be covered by SMRR which has alignment requirements. */
	tsegsize = 1;
	mmiosize = 1024;

	reclaim = false;
	tolud = MIN(4096 - mmiosize, tom);
	if ((tom - tolud) > 64) {
		reclaim = true;
	}
	if (reclaim) {
		tolud = tolud & ~0x3f;
		tom   = tom   & ~0x3f;
		reclaimbase = MAX(4096, tom);
		reclaimlimit = reclaimbase + (MIN(4096, tom) - tolud) - 0x40;
	}
	touud = tom;
	if (reclaim) {
		touud = reclaimlimit + 64;
	}

	gfxbase  = tolud   - gfxsize;
	gttbase  = gfxbase - gttsize;
	tsegbase = gttbase - tsegsize;

	/* Program the regs */
	pci_write_config16(HOST_BRIDGE, TOLUD, (u16)(tolud << 4));
	pci_write_config16(HOST_BRIDGE, TOM,   (u16)(tom   >> 6));
	if (reclaim) {
		pci_write_config16(HOST_BRIDGE, 0x98, (u16)(reclaimbase  >> 6));
		pci_write_config16(HOST_BRIDGE, 0x9a, (u16)(reclaimlimit >> 6));
	}
	pci_write_config16(HOST_BRIDGE, TOUUD, (u16)(touud));
	pci_write_config32(HOST_BRIDGE, GBSM, gfxbase  << 20);
	pci_write_config32(HOST_BRIDGE, BGSM, gttbase  << 20);
	pci_write_config32(HOST_BRIDGE, TSEG, tsegbase << 20);

	u8 reg8 = pci_read_config8(HOST_BRIDGE, ESMRAMC);
	reg8 &= ~0x07;
	reg8 |= (0 << 1) | (1 << 0); /* 1M and TSEG_Enable */
	pci_write_config8(HOST_BRIDGE, ESMRAMC, reg8);

	printk(BIOS_DEBUG, "GBSM (igd) = verified %08x (written %08x)\n",
		pci_read_config32(HOST_BRIDGE, GBSM), gfxbase  << 20);
	printk(BIOS_DEBUG, "BGSM (gtt) = verified %08x (written %08x)\n",
		pci_read_config32(HOST_BRIDGE, BGSM), gttbase  << 20);
	printk(BIOS_DEBUG, "TSEG (smm) = verified %08x (written %08x)\n",
		pci_read_config32(HOST_BRIDGE, TSEG), tsegbase << 20);
}

static void sdram_enhancedmode(struct sysinfo *s)
{
	u8 reg8, ch, r, fsb_freq, ddr_freq;
	u32 mask32, reg32;
	MCHBAR8_OR(C0ADDCSCTRL, 1);
	MCHBAR8_OR(C0REFRCTRL + 3, 1);
	mask32 = (0x1f << 15) | (0x1f << 10) | (0x1f << 5) | 0x1f;
	reg32  = (0x1e << 15) | (0x10 << 10) | (0x1e << 5) | 0x10;
	MCHBAR32_AND_OR(WRWMCONFIG, ~mask32, reg32);
	MCHBAR8(C0DITCTRL + 1)  = 2;
	MCHBAR16(C0DITCTRL + 2) = 0x0804;
	MCHBAR16(C0DITCTRL + 4) = 0x2010;
	MCHBAR8(C0DITCTRL + 6)  = 0x40;
	MCHBAR16(C0DITCTRL + 8) = 0x091c;
	MCHBAR8(C0DITCTRL + 10) = 0xf2;
	MCHBAR8_OR(C0BYPCTRL, 1);
	MCHBAR8_OR(C0CWBCTRL, 1);
	MCHBAR16_OR(C0ARBSPL, 0x0100);

	pci_or_config8(HOST_BRIDGE, 0xf0, 1);
	MCHBAR32(SBCTL)  = 0x00000002;
	MCHBAR32(SBCTL2) = 0x20310002;
	MCHBAR32(SLIMCFGTMG) = 0x02020302;
	MCHBAR32(HIT0) = 0x001f1806;
	MCHBAR32(HIT1) = 0x01102800;
	MCHBAR32(HIT2) = 0x07000000;
	MCHBAR32(HIT3) = 0x01014010;
	MCHBAR32(HIT4) = 0x0f038000;
	pci_and_config8(HOST_BRIDGE, 0xf0, ~1);

	u32 nranks, curranksize, maxranksize, dra;
	u8 rankmismatch;
	static const u8 drbtab[10] = {0x4, 0x2, 0x8, 0x4, 0x8, 0x4, 0x10, 0x8, 0x20, 0x10};

	nranks = 0;
	curranksize = 0;
	maxranksize = 0;
	rankmismatch = 0;

	FOR_EACH_POPULATED_RANK(s->dimms, ch, r) {
		nranks++;
		dra = (u8) ((MCHBAR32(C0DRA01) >> (8 * r)) & 0x7f);
		curranksize = drbtab[dra];
		if (maxranksize == 0) {
			maxranksize = curranksize;
		}
		if (curranksize != maxranksize) {
			rankmismatch = 1;
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
	MCHBAR8_AND_OR(CHDECMISC, ~0xfc, reg8 & 0xfc);
	MCHBAR32_AND(NOACFGBUSCTL, ~0x80000000);

	MCHBAR32(HTBONUS0) = 0x0000000f;
	MCHBAR8_OR(C0COREBONUS + 4, 1);

	MCHBAR32_AND(HIT3, ~0x0e000000);
	MCHBAR32_AND_OR(HIT4, ~0x000c0000, 0x00040000);

	u32 clkcx[2][2][3] = {
	{
		{0x00000000, 0x0c080302, 0x08010204},	/* FSB = 667, DDR = 667 */
		{0x02040000, 0x08100102, 0x00000000},	/* FSB = 667, DDR = 800 */
	},
	{
		{0x18000000, 0x3021060c, 0x20010208},	/* FSB = 800, DDR = 667 */
		{0x00000000, 0x0c090306, 0x00000000},	/* FSB = 800, DDR = 800 */
	}
	};

	fsb_freq = s->selected_timings.fsb_clock;
	ddr_freq = s->selected_timings.mem_clock;

	MCHBAR32(CLKXSSH2X2MD)      = clkcx[fsb_freq][ddr_freq][0];
	MCHBAR32(CLKXSSH2X2MD + 4)  = clkcx[fsb_freq][ddr_freq][1];
	MCHBAR32(CLKXSSH2MCBYP + 4) = clkcx[fsb_freq][ddr_freq][2];

	MCHBAR8_AND(HIT4, ~0x02);
}

static void sdram_periodic_rcomp(void)
{
	MCHBAR8_AND(COMPCTRL1, ~0x02);
	while ((MCHBAR32(COMPCTRL1) & 0x80000000) > 0) {
		;
	}
	MCHBAR16_AND(CSHRMISCCTL, ~0x3000);
	MCHBAR8_OR(CMNDQFIFORST, 0x80);
	MCHBAR16_AND_OR(XCOMPDFCTRL, ~0x0f, 0x09);

	MCHBAR8_OR(COMPCTRL1, 0x82);
}

static void sdram_new_trd(struct sysinfo *s)
{
	u8 pidelay, i, j, k, cc, trd_perphase[5];
	u8 bypass, freqgb, trd, reg8, txfifo;
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
		rcvendelay  = ((u32)((s->coarsedelay  >> (i << 1)) & 3) * (u32)(tmclk));
		rcvendelay += ((u32)((s->readptrdelay >> (i << 1)) & 3) * (u32)(tmclk) / 2);
		rcvendelay += ((u32)((s->mediumphase  >> (i << 1)) & 3) * (u32)(tmclk) / 4);
		rcvendelay +=  (u32)(pidelay * s->pi[i]);
		maxrcvendelay = MAX(maxrcvendelay, rcvendelay);
	}

	if ((MCHBAR8(HMBYPCP + 3) == 0xff) && (MCHBAR8(HMCCMC) & 0x80)) {
		bypass = 1;
	} else {
		bypass = 0;
	}

	txfifo = 0;
	reg8 = (MCHBAR8(CSHRFIFOCTL) & 0x0e) >> 1;
	txfifo = txfifo_lut[reg8] & 0x07;

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

	MCHBAR16_AND_OR(C0STATRDCTRL, ~0x1f00, trd << 8);
}

static void sdram_powersettings(struct sysinfo *s)
{
	u8 j;
	u32 reg32;

	/* Thermal sensor */
	MCHBAR8(TSC1) = 0x9b;
	MCHBAR32_AND_OR(TSTTP, ~0x00ffffff, 0x1d00);
	MCHBAR8(THERM1) = 0x08;
	MCHBAR8(TSC3) = 0x00;
	MCHBAR8_AND_OR(TSC2, ~0x0f, 0x04);
	MCHBAR8_AND_OR(THERM1, ~1, 1);
	MCHBAR8_AND_OR(TCO, ~0x80, 0x80);

	/* Clock gating */
	MCHBAR32_AND(PMMISC, ~0x00040001);
	MCHBAR8_AND(SBCTL3   + 3, ~0x80);
	MCHBAR8_AND(CISDCTRL + 3, ~0x80);
	MCHBAR16_AND(CICGDIS, ~0x1fff);
	MCHBAR32_AND(SBCLKGATECTRL, ~0x0001ffff);
	MCHBAR16_AND(HICLKGTCTL, ~0x03ff & 0x06);
	MCHBAR32_AND_OR(HTCLKGTCTL, ~0xffffffff, 0x20);
	MCHBAR8_AND(TSMISC, ~1);
	MCHBAR8(C0WRDPYN) = s->selected_timings.CAS - 1 + 0x15;
	MCHBAR16_AND_OR(CLOCKGATINGI,  ~0x07fc, 0x0040);
	MCHBAR16_AND_OR(CLOCKGATINGII, ~0x0fff, 0x0d00);
	MCHBAR16_AND(CLOCKGATINGIII,   ~0x0d80);
	MCHBAR16(GTDPCGC + 2) = 0xffff;

	/* Sequencing */
	MCHBAR32(HPWRCTL1) = (MCHBAR32(HPWRCTL1) & ~0x1fffffff) | 0x1f643fff;
	MCHBAR32(HPWRCTL2) = (MCHBAR32(HPWRCTL2) & ~0xffffff7f) | 0x02010000;
	MCHBAR16(HPWRCTL3) = (MCHBAR16(HPWRCTL3) & ~0x7000) | (3 << 12);

	/* Power */
	MCHBAR32(GFXC3C4)  = (MCHBAR32(GFXC3C4)  & ~0xffff0003) | 0x10100000;
	MCHBAR32(PMDSLFRC) = (MCHBAR32(PMDSLFRC) & ~0x0001bff7) | 0x00000078;

	if (s->selected_timings.fsb_clock == FSB_CLOCK_667MHz)
		MCHBAR16_AND_OR(PMMSPMRES, ~0x03ff, 0x00c8);
	else
		MCHBAR16_AND_OR(PMMSPMRES, ~0x03ff, 0x0100);

	j = (s->selected_timings.mem_clock == MEM_CLOCK_667MHz) ? 0 : 1;

	MCHBAR32_AND_OR(PMCLKRC, ~0x01fff37f, 0x10810700);
	MCHBAR8_AND_OR(PMPXPRC,  ~0x07, 1);
	MCHBAR8_AND(PMBAK, ~0x02);

	static const u16 ddr2lut[2][4][2] = {
	{
		{0x0000, 0x0000},
		{0x019A, 0x0039},
		{0x0099, 0x1049},
		{0x0000, 0x0000},
	},
	{
		{0x0000, 0x0000},
		{0x019A, 0x0039},
		{0x0099, 0x1049},
		{0x0099, 0x2159},
	},
	};

	MCHBAR16(C0C2REG)  = 0x7a89;
	MCHBAR8(SHC2REGII) = 0xaa;
	MCHBAR16(SHC2REGII + 1) = ddr2lut[j][s->selected_timings.CAS - 3][1];
	MCHBAR16_AND_OR(SHC2REGI, ~0x7fff, ddr2lut[j][s->selected_timings.CAS - 3][0]);
	MCHBAR16_AND_OR(CLOCKGATINGIII, ~0xf000, 0xf000);
	MCHBAR8(CSHWRIOBONUSX) = (MCHBAR8(CSHWRIOBONUSX) & ~0x77) | (4 << 4 | 4);

	reg32 = s->nodll ? 0x30000000 : 0;

	/* FIXME: Compacting this results in changes to the binary */
	MCHBAR32(C0COREBONUS)  = (MCHBAR32(C0COREBONUS)  & ~0x0f000000) | 0x20000000 | reg32;

	MCHBAR32_AND_OR(CLOCKGATINGI, ~0x00f00000, 0x00f00000);
	MCHBAR32_AND_OR(CLOCKGATINGII - 1, ~0x001ff000, 0xbf << 20);
	MCHBAR16_AND_OR(SHC3C4REG2, ~0x1f7f, (0x0b << 8) | (7 << 4) | 0x0b);
	MCHBAR16(SHC3C4REG3) = 0x3264;
	MCHBAR16_AND_OR(SHC3C4REG4, ~0x3f3f, (0x14 << 8) | 0x0a);

	MCHBAR32_OR(C1COREBONUS, 0x80002000);
}

static void sdram_programddr(void)
{
	MCHBAR16_AND_OR(CLOCKGATINGII,  ~0x03ff, 0x0100);
	MCHBAR16_AND_OR(CLOCKGATINGIII, ~0x003f, 0x0010);
	MCHBAR16_AND_OR(CLOCKGATINGI,   ~0x7000, 0x2000);

	MCHBAR8_AND(CSHRPDCTL,     ~0x0e);
	MCHBAR8_AND(CSHRWRIOMLNS,  ~0x0c);
	MCHBAR8_AND(C0MISCCTLy(0), ~0x0e);
	MCHBAR8_AND(C0MISCCTLy(1), ~0x0e);
	MCHBAR8_AND(C0MISCCTLy(2), ~0x0e);
	MCHBAR8_AND(C0MISCCTLy(3), ~0x0e);
	MCHBAR8_AND(C0MISCCTLy(4), ~0x0e);
	MCHBAR8_AND(C0MISCCTLy(5), ~0x0e);
	MCHBAR8_AND(C0MISCCTLy(6), ~0x0e);
	MCHBAR8_AND(C0MISCCTLy(7), ~0x0e);
	MCHBAR8_AND(CSHRWRIOMLNS,  ~0x02);

	MCHBAR16_AND(CSHRMISCCTL, ~0x0400);
	MCHBAR16_AND(CLOCKGATINGIII, ~0x0dc0);
	MCHBAR8_AND(C0WRDPYN, ~0x80);
	MCHBAR32_AND(C0COREBONUS, ~(1 << 22));
	MCHBAR16_AND(CLOCKGATINGI,  ~0x80fc);
	MCHBAR16_AND(CLOCKGATINGII, ~0x0c00);

	MCHBAR8_AND(CSHRPDCTL,     ~0x0d);
	MCHBAR8_AND(C0MISCCTLy(0), ~1);
	MCHBAR8_AND(C0MISCCTLy(1), ~1);
	MCHBAR8_AND(C0MISCCTLy(2), ~1);
	MCHBAR8_AND(C0MISCCTLy(3), ~1);
	MCHBAR8_AND(C0MISCCTLy(4), ~1);
	MCHBAR8_AND(C0MISCCTLy(5), ~1);
	MCHBAR8_AND(C0MISCCTLy(6), ~1);
	MCHBAR8_AND(C0MISCCTLy(7), ~1);

	MCHBAR32_AND_OR(C0STATRDCTRL, ~0x00700000, 3 << 20);
	MCHBAR32_AND(C0COREBONUS, ~0x00100000);
	MCHBAR8_OR(C0DYNSLVDLLEN, 0x1e);
	MCHBAR8_OR(C0DYNSLVDLLEN2, 0x03);
	MCHBAR32_AND_OR(SHCYCTRKCKEL, ~0x0c000000, 0x04000000);
	MCHBAR16_OR(C0STATRDCTRL, 0x6000);
	MCHBAR32_OR(C0CKECTRL, 0x00010000);
	MCHBAR8_OR(C0COREBONUS, 0x10);
	MCHBAR32_OR(CLOCKGATINGI - 1, 0xf << 24);
	MCHBAR8_OR(CSHWRIOBONUS, 0x07);
	MCHBAR8_OR(C0DYNSLVDLLEN, 0xc0);
	MCHBAR8_OR(SHC2REGIII, 7);
	MCHBAR16_AND_OR(SHC2MINTM, ~0xffff, 0x0080);
	MCHBAR8_AND_OR(SHC2IDLETM, ~0xff, 0x10);
	MCHBAR16_OR(C0COREBONUS, 0x01e0);
	MCHBAR8_OR(CSHWRIOBONUS, 0x18);
	MCHBAR8_OR(CSHRMSTDYNDLLENB, 0x0d);
	MCHBAR16_OR(SHC3C4REG1, 0x0a3f);
	MCHBAR8_OR(C0STATRDCTRL, 3);
	MCHBAR8_AND_OR(C0REFRCTRL2, ~0xff, 0x4a);
	MCHBAR8_AND(C0COREBONUS + 4, ~0x60);
	MCHBAR16_OR(C0DYNSLVDLLEN, 0x0321);
}

static void sdram_programdqdqs(struct sysinfo *s)
{
	u16 mdclk, tpi, refclk, dqdqs_out, dqdqs_outdelay, dqdqs_delay;
	u32 coretomcp, txdelay, tmaxunmask, tmaxpi;
	u8 repeat, halfclk, feature, reg8, push;
	u16 cwb, pimdclk;
	u32 reg32;
	static const u8 txfifotab[8] = {0, 7, 6, 5, 2, 1, 4, 3};

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

	coretomcp = ((MCHBAR8(C0ADDCSCTRL) >> 2) & 0x3) + 1;
	coretomcp *= mdclk;

	reg8 = (MCHBAR8(CSHRFIFOCTL) & 0x0e) >> 1;

	while (repeat) {
		txdelay = mdclk * (
				((MCHBAR16(C0GNT2LNCH1) >> 8) & 0x7) +
				(MCHBAR8(C0WRDATACTRL) & 0xf) +
				(MCHBAR8(C0WRDATACTRL + 1) & 0x1)
				) +
				txfifotab[reg8]*(mdclk / 2) +
				coretomcp +
				refclk +
				cwb;
		halfclk = (MCHBAR8(C0MISCCTL) >> 1) & 0x1;
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
				MCHBAR32_AND(C0COREBONUS, ~(1 << 23));
			}
			feature = 1;
			repeat = 0;
		} else {
			repeat--;
			MCHBAR32_OR(C0COREBONUS, 1 << 23);
			cwb = 2 * mdclk;
		}
	}

	if (!feature) {
		MCHBAR8(CLOCKGATINGI) = MCHBAR8(CLOCKGATINGI) & ~0x3;
		return;
	}
	MCHBAR8_OR(CLOCKGATINGI, 3);
	MCHBAR16_AND_OR(CLOCKGATINGIII, ~0xf000, pimdclk << 12);
	MCHBAR8_AND_OR(CSHWRIOBONUSX, ~0x77, (push << 4) | push);
	MCHBAR32_AND_OR(C0COREBONUS, ~0x0f000000, 0x03000000);
}

/**
 * @param boot_path: 0 = normal, 1 = reset, 2 = resume from s3
 */
void sdram_initialize(int boot_path, const u8 *spd_addresses)
{
	struct sysinfo si;
	const char *boot_str[] = {"Normal", "Reset", "Resume"};

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

	MCHBAR16_OR(CPCTL, 1 << 15);

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
		while ((MCHBAR8(COMPCTRL1) & 1) != 0)
			;
	}

	sdram_mmap(&si);
	PRINTK_DEBUG("Done mmap\n");

	/* Enable DDR IO buffer */
	MCHBAR8_AND_OR(C0IOBUFACTCTL, ~0x3f, 0x08);
	MCHBAR8_OR(C0RSTCTL, 1);

	sdram_rcompupdate(&si);
	PRINTK_DEBUG("Done RCOMP update\n");

	MCHBAR8_OR(HIT4, 2);

	if (si.boot_path != BOOT_PATH_RESUME) {
		MCHBAR32_OR(C0CKECTRL, 1 << 27);

		sdram_jedecinit(&si);
		PRINTK_DEBUG("Done MRS\n");
	}

	sdram_misc(&si);
	PRINTK_DEBUG("Done misc\n");

	sdram_zqcl(&si);
	PRINTK_DEBUG("Done zqcl\n");

	if (si.boot_path != BOOT_PATH_RESUME) {
		MCHBAR32_OR(C0REFRCTRL2, 3 << 30);
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
	MCHBAR32_OR(C0REFRCTRL2, 1 << 30);

	/* Tell ICH7 that we're done */
	pci_and_config8(PCI_DEV(0, 0x1f, 0), 0xa2, (u8)~(1 << 7));

	/* Tell northbridge we're done */
	pci_or_config8(HOST_BRIDGE, 0xf4, 1);

	printk(BIOS_DEBUG, "RAM initialization finished.\n");
}
