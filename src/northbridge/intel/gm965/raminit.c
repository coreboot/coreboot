/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - RAM Initialization
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS
 *
 * BIOS functions (RAMINIT region FFF00000-FFF03900, 55 functions):
 *   FFF00240 - raminit main entry (500 bytes)
 *   FFF01D4F - main sequencer (POST 0xFF17-0xFF43) <- actual execution path
 *   FFF024C9 - cold boot core (SPD -> freq -> timing -> rank count)
 *   FFF0200E - DIMM detect and validate (329 bytes)
 *   FFF02157 - frequency/multiplier selection (392 bytes)
 *   FFF022DF - timing parameter calculation (490 bytes)
 *   FFF02A7A - EPD address decode programming (1603 bytes)
 *   FFF03286 - channel population + EPD enable (112 bytes)
 *   FFF00B1F - DRAM init command sequence (88 bytes)
 *   FFF009AD - CLKCFG programming (188 bytes)
 *   FFF00A8E - DCC programming (140 bytes)
 *
 * BIOS functions (RAMINIT3 region FFFF3000-FFFF5700, 29 functions):
 *   FFFF3220 - RAMINIT3 main entry (458 bytes)
 *   FFFF5115 - secondary sequencer (POST 0xFF17-0xFF43)
 *   FFFF3C49 - RCOMP/DCC configuration (296 bytes)
 *   FFFF408F - DRAM timing register programming (881 bytes)
 *   FFFF4573 - DDR2 JEDEC EMRS1 sequence (561 bytes)
 *   FFFF4F9D - receive-enable training orchestrator (283 bytes)
 *   FFFF4E70 - per-channel REC training (3-phase)
 *
 * NOTE: The RAMINIT copy (FFF0xxxx) is the BIOS's actual execution
 * path and contains 55 functions including EPD address decode
 * programming that is critical for dual-channel operation.  The
 * RAMINIT3 copy (FFFF3xxx) is a stripped-down version with only 29
 * functions that omits the EPD programming entirely.  This file
 * was originally based on RAMINIT3 and has been augmented with the
 * EPD functions from the complete RAMINIT copy.
 *
 * GM965 is DDR2-only (no DDR3). Two DIMMs max (1 per channel).
 *
 * Reference: coreboot GM45 raminit.c (DDR2 paths)
 */

#include <stdint.h>
#include <string.h>
#include <arch/cpu.h>
#include <lib.h>
#include <device/mmio.h>
#include <device/i2c_simple.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <cf9_reset.h>
#include <northbridge/intel/gm965/gm965.h>
#include <southbridge/intel/i82801hx/i82801hx.h>
#include <mrc_cache.h>

/* MRC cache version for GM965 */
#define MRC_CACHE_VERSION 1

/* Cache structure for storing DRAM init state to SPI flash via MRC cache */
struct gm965_mrc_cache {
	uint32_t cpu_id;
	uint16_t spd_crc[4];		/* Unique CRC for each DIMM slot */
	dimminfo_t dimms[4];		/* Decoded DIMM attributes */
	timings_t timings;		/* Computed timings */
	uint8_t rec_coarse[2];		/* Receive-enable training coarse */
	uint8_t rec_coarse_low[2];	/* Receive-enable training coarse_low */
	uint8_t rec_fine[2];		/* Receive-enable training fine */
	uint8_t raw_spd[4][SPD_SIZE_MAX_DDR2]; /* Full SPD for identification */
};

/* Read SPD unique identification bytes and compute CRC */
static u16 spd_read_unique_crc(u8 i2c_addr)
{
	u8 spd[128] = {0};
	/* Read bytes 64-72 (9 bytes) */
	if (i2c_eeprom_read(i2c_addr, 64, 9, spd + 64) < 0) {
		for (int i = 64; i <= 72; i++)
			spd[i] = smbus_read_byte(i2c_addr, i);
	}
	/* Read bytes 93-98 (6 bytes) */
	if (i2c_eeprom_read(i2c_addr, 93, 6, spd + 93) < 0) {
		for (int i = 93; i <= 98; i++)
			spd[i] = smbus_read_byte(i2c_addr, i);
	}
	return spd_ddr2_calc_unique_crc(spd, sizeof(spd));
}

/* Copy cached data into sysinfo and recompute derived fields */
static void copy_cache_to_si(sysinfo_t *si, const struct gm965_mrc_cache *cache)
{
	memcpy(si->dimms, cache->dimms, sizeof(cache->dimms));
	memcpy(&si->timings, &cache->timings, sizeof(cache->timings));
	memcpy(si->rec_coarse, cache->rec_coarse, sizeof(cache->rec_coarse));
	memcpy(si->rec_coarse_low, cache->rec_coarse_low, sizeof(cache->rec_coarse_low));
	memcpy(si->rec_fine, cache->rec_fine, sizeof(cache->rec_fine));
	memcpy(si->raw_spd, cache->raw_spd, sizeof(cache->raw_spd));

	/* Compute dimm_count and channels */
	si->dimm_count = 0;
	int ch0_pop = 0, ch1_pop = 0;
	for (int i = 0; i < 4; i++) {
		if (si->dimms[i].present) {
			si->dimm_count++;
			if (i < 2)
				ch0_pop = 1;
			else
				ch1_pop = 1;
		}
	}
	si->channels = (ch0_pop ? 1 : 0) + (ch1_pop ? 1 : 0);
}
/* MCHBAR_BASE needed for RCOMP table direct pointer access */
#ifndef MCHBAR_BASE
#define MCHBAR_BASE   CONFIG_FIXED_MCHBAR_MMIO_BASE
#endif

/*
 * SPD byte offsets and memory type constants are provided by
 * <spd.h> (included via <device/dram/ddr2.h>).  SPD decoding
 * is handled by the common DDR2 library (spd_decode_ddr2()).
 */

/*
 * SPD slave addresses are stored in sysinfo_t.spd_addr_map, populated by
 * mainboard_get_spd_map() at the start of raminit().
 * Index maps to logical DIMM slots: 0 = ch0 DIMM0, 2 = ch1 DIMM0.
 * Slots 1 and 3 are kept unused on GM965. A zero entry means the slot is
 * unpopulated and will be skipped.
 */

/* ================================================================== */
/* Timing lookup tables                                               */
/* ================================================================== */

/*
 * DDR2 clock period in 1/256 ns units (matching coreboot encoding).
 * Index: mem_clock_t value.
 */
/*
 * DDR2 clock period in 1/256 ns units.
 * Indexed by mem_clock_t: [1]=DDR2-533, [2]=DDR2-667.
 *
 * DDR2-533 = 266 MHz clock -> tCK = 3.75 ns = 960/256 ns
 * DDR2-667 = 333 MHz clock -> tCK = 3.0 ns = 768/256 ns
 */
static const unsigned int tCK_from_clock[] = {
	[MEM_CLOCK_533MT] = TCK_266MHZ,   /* 3.75 ns */
	[MEM_CLOCK_667MT] = TCK_333MHZ,   /* 3.0 ns */
};

/*
 * tRFC lookup table: indexed by [mem_clock - 1][chip_capacity].
 * Values in clock cycles = ceil(tRFC_ns / tCK_ns).
 * Chip capacity: 0=256Mb, 1=512Mb, 2=1Gb, 3=2Gb.
 *
 * tRFC_min from JEDEC: 256Mb=75ns, 512Mb=105ns, 1Gb=127.5ns, 2Gb=195ns.
 * Verified against inteltool: DRT3[20:13] = 43 for 667MT + 1Gb chips.
 */
static const unsigned int tRFC_table[2][4] = {
	/*            256Mb  512Mb  1Gb   2Gb   */
	/* 533MT */  {  20,    28,   34,   52 },
	/* 667MT */  {  25,    35,   43,   65 },
};

/*
 * tFAW fixed values per memory clock (BIOS uses fixed values, not page-dependent).
 * BIOS: FFFF408F - sVar2 lookup: mc=1->14, mc=2->17, mc=3->11
 */
static const unsigned int tFAW_fixed[] = {
	[0]               = 11,
	[MEM_CLOCK_533MT] = 14,
	[MEM_CLOCK_667MT] = 17,
};

/*
 * CxDRT4 ROM table (BIOS file offset 0x1757D0).
 * Each entry is the full register value, masked with 0x10200 (preserve bits 16,9).
 * Indexed by mem_clock.
 */
static const uint32_t drt4_rom_table[] = {
	[0]               = 0x00000000,
	[MEM_CLOCK_533MT] = 0x390c2850,
	[MEM_CLOCK_667MT] = 0x414e3064,
};

/*
 * CxDRT5 byte table (BIOS file offset 0x1757E0).
 * Values go into bits [19:12] of CxDRT5. Indexed by mem_clock.
 */
static const uint8_t drt5_rom_bytes[] = {
	[0]               = 0x00,
	[MEM_CLOCK_533MT] = 0x50,
	[MEM_CLOCK_667MT] = 0x64,
};

/*
 * CxDRT0 BtB_WtR tWTR equivalent lookup, indexed by mem_clock.
 * BIOS: abStack_8[] = {2, 2, 3, 3} - used in WtR formula.
 */
static const uint8_t drt0_twtr_lut[] = {
	[0]               = 2,
	[MEM_CLOCK_533MT] = 2,
	[MEM_CLOCK_667MT] = 3,
};

/* ================================================================== */
/* SPD reading and DIMM detection                                     */
/* ================================================================== */

/*
 * detect_dimms() - Scan SPD and populate sysinfo DIMM structures
 *
 * Reads only the first 64 bytes of SPD (timing/geometry data) for
 * boot speed -- manufacturer info at bytes 64-127 is not needed for
 * raminit.  Uses i2c_eeprom_read() block transfer where possible,
 * falling back to individual smbus_read_byte() calls.  The raw SPD
 * data is decoded by the common DDR2 library (spd_decode_ddr2()).
 *
 * BIOS equivalent: FFF0200E / FFFF48A7 (POST 0xFF05-0xFF08)
 * coreboot equivalent: collect_dimm_config()
 */
static void detect_dimms(sysinfo_t *si)
{
	int slot, j;

	si->dimm_count = 0;
	si->channels = 0;

	for (slot = 0; slot < 4; slot++) {
		dimminfo_t *d = &si->dimms[slot];
		u8 raw_spd[SPD_SIZE_MAX_DDR2] = {};
		struct dimm_attr_ddr2_st decoded;

		/* Skip slots not wired to an SPD device on this board */
		if (!si->spd_addr_map[slot]) {
			d->present = 0;
			continue;
		}

		/* Quick presence check: read memory type byte first */
		if (smbus_read_byte(si->spd_addr_map[slot], SPD_MEMORY_TYPE)
		    != SPD_MEMORY_TYPE_SDRAM_DDR2) {
			printk(BIOS_DEBUG, "SPD slot %d addr 0x%02x: "
			       "not DDR2, skipping\n",
			       slot, si->spd_addr_map[slot]);
			d->present = 0;
			continue;
		}

		/*
		 * Read full 128 bytes of SPD. The timing/geometry data
		 * is in bytes 0-63, but we'll need identification fields
		 * from bytes 64-98 later for SMBIOS.
		 */
		if (i2c_eeprom_read(si->spd_addr_map[slot], 0, SPD_SIZE_MAX_DDR2,
				    raw_spd) != SPD_SIZE_MAX_DDR2) {
			printk(BIOS_DEBUG, "SPD slot %d: i2c block read "
			       "failed, falling back to byte reads\n", slot);
			for (j = 0; j < SPD_SIZE_MAX_DDR2; j++)
				raw_spd[j] = smbus_read_byte(
					si->spd_addr_map[slot], j);
		}

		if (spd_decode_ddr2(&decoded, raw_spd) != SPD_STATUS_OK) {
			printk(BIOS_WARNING, "DIMM%d: SPD decode failed, "
			       "skipping\n", slot);
			d->present = 0;
			continue;
		}

		if (CONFIG(DEBUG_RAM_SETUP))
			dram_print_spd_ddr2(&decoded);

		/* GM965 does not support ECC or registered memory */
		if (decoded.flags.is_ecc)
			die("ECC memory not supported by GM965\n");
		if (spd_dimm_is_registered_ddr2(decoded.dimm_type))
			die("Registered memory not supported by GM965\n");

		d->present = 1;
		si->dimm_count++;

		/* Geometry from decoded struct */
		d->dual_rank = (decoded.ranks > 1) ? 1 : 0;
		d->rows = decoded.row_bits;
		d->cols = decoded.col_bits;
		d->banks = decoded.banks;
		d->x16 = (decoded.width == 16) ? 1 : 0;

		/* Page size = chip_width_bytes * 2^cols */
		unsigned int chip_width_bytes = decoded.width / 8;
		if (chip_width_bytes == 0)
			chip_width_bytes = 1;
		d->page_size = chip_width_bytes * (1 << d->cols);

		/* Rank capacity from library */
		d->rank_capacity_mb = decoded.ranksize_mb;

		/* CAS and per-CAS cycle times (already in 1/256 ns) */
		d->cas_supported = decoded.cas_supported;
		memcpy(d->cycle_time, decoded.cycle_time,
		       sizeof(d->cycle_time));

		/* Timing parameters in 1/256 ns from library */
		d->tRCD = decoded.tRCD;
		d->tRP  = decoded.tRP;
		d->tRAS = decoded.tRAS;
		d->tWR  = decoded.tWR;
		d->tRRD = decoded.tRRD;
		d->tRTP = decoded.tRTP;

		printk(BIOS_DEBUG, "DIMM%d: %dMB/rank %s rows=%d cols=%d "
		       "banks=%d CAS=0x%02x tCK=%u (1/256 ns)\n",
		       slot, d->rank_capacity_mb,
		       d->dual_rank ? "2R" : "1R",
		       d->rows, d->cols, d->banks,
		       d->cas_supported,
		       d->cycle_time[spd_get_msbs(d->cas_supported)]);

		/* Save raw SPD for later SMBIOS extraction */
		memcpy(si->raw_spd[slot], raw_spd, SPD_SIZE_MAX_DDR2);
	}

	if (si->dimm_count == 0)
		die("No DIMMs detected (POST 0xE0)");

	/*
	 * Determine channel mode.
	 * Ch0 = slots 0,1; Ch1 = slots 2,3.
	 * X61 typically has 1 slot per channel (slot 0 and slot 2).
	 */
	int ch0_pop = si->dimms[0].present || si->dimms[1].present;
	int ch1_pop = si->dimms[2].present || si->dimms[3].present;

	if (ch0_pop && ch1_pop) {
		si->channels = 2;
		si->timings.channel_mode = CHANNEL_MODE_DUAL_INTERLEAVED;
	} else {
		si->channels = 1;
		si->timings.channel_mode = CHANNEL_MODE_SINGLE;
	}
}

/* ================================================================== */
/* Frequency and CAS latency selection                                */
/* ================================================================== */

/*
 * read_fsb_clock() - Read FSB frequency from CLKCFG register
 *
 * BIOS equivalent: reads MCHBAR+0xC00 bits [2:0]
 */
static fsb_clock_t read_fsb_clock(void)
{
	uint32_t clkcfg = mchbar_read32(CLKCFG_MCHBAR);
	uint8_t fsb_code = clkcfg & CLKCFG_FSBCLK_MASK;

	/* GM965 FSB codes: 1=800MHz, 2=667MHz */
	switch (fsb_code) {
	case 1:  return FSB_CLOCK_533MHz;
	case 2:  return FSB_CLOCK_800MHz;
	case 3:  return FSB_CLOCK_667MHz;
	default: return FSB_CLOCK_800MHz;
	}
}

/*
 * select_frequency_and_cas() - Find highest common clock and minimum valid CAS
 *
 * Three-phase algorithm:
 *
 * Phase 1 - characterise the DIMM population.
 *   tCKmin_common: max(tCK_min[i]) over all DIMMs - the slowest minimum clock
 *   period across the population, which sets the fastest frequency every DIMM
 *   can sustain simultaneously.
 *   tAA_needed: max(max_cas[i] * tCK_min[i]) - the absolute-time lower bound
 *   on CAS access latency.  In DDR2 a DIMM is characterised at its highest
 *   supported CAS running at tCK_min, so their product is the minimum tAA the
 *   system must honour.
 *   cas_mask: bitwise-AND of every DIMM's CAS latency bitmask - only CAS
 *   values that every DIMM in the system supports.
 *
 * Phase 2 - select the memory clock.
 *   Walk 800->667->533 MT/s; accept the first candidate where:
 *     (a) tCK_clock >= tCKmin_common  (all DIMMs can sustain the frequency)
 *     (b) clock_mhz <= max_mem_mhz   (within the FSB-derived ceiling)
 *   Clock compatibility is assessed on tCK_min alone, independently of CAS.
 *
 * Phase 3 - derive CAS at the chosen clock from absolute time.
 *   cas_needed = ceil(tAA_needed / tCK_clock)
 *   Then pick the lowest CAS value in cas_mask that satisfies CAS >= cas_needed.
 *   If no such CAS exists at this clock (rare edge case), fall back to the next
 *   slower clock and repeat phase 3.
 *
 * BIOS equivalent: FFF02157 / FFFF49F1 (POST 0xFF09-0xFF10)
 * coreboot equivalent: find_common_clock_cas()
 */
static void select_frequency_and_cas(sysinfo_t *si)
{
	int slot;
	unsigned int cas_mask = 0xff;    /* intersected CAS bitmask */
	unsigned int tCKmin_common = 0;  /* max tCK_min across DIMMs (1/256 ns) */
	unsigned int tAA_needed = 0;     /* worst-case min access time (1/256 ns) */

	si->timings.fsb_clock = read_fsb_clock();

	/*
	 * Phase 1: characterise the DIMM population.
	 *
	 * Timing values are already decoded to 1/256 ns by the common
	 * DDR2 SPD library (spd_decode_ddr2), so no manual tCK decoding
	 * is needed here.
	 */
	for (slot = 0; slot < 4; slot++) {
		dimminfo_t *d = &si->dimms[slot];
		if (!d->present)
			continue;

		/* Common supported CAS set */
		cas_mask &= d->cas_supported;

		/* Highest CAS this DIMM supports */
		unsigned int max_cas = spd_get_msbs(d->cas_supported);

		/* tCK at max CAS - already in 1/256 ns from library */
		unsigned int tCK = d->cycle_time[max_cas];
		if (tCK > tCKmin_common)
			tCKmin_common = tCK;

		/*
		 * Absolute minimum access time for this DIMM:
		 * tAA_min = max_cas * tCK_min  (both in 1/256 ns units).
		 * The DIMM is rated to run max_cas at tCK_min, so this is
		 * the smallest tAA the controller must supply.
		 */
		unsigned int tAA_dimm = max_cas * tCK;
		if (tAA_dimm > tAA_needed)
			tAA_needed = tAA_dimm;
	}

	if (cas_mask == 0)
		die("No common CAS latency (POST 0xEC)");

	/*
	 * Phase 2: Memory clock ceiling.
	 *
	 * EDS 3.1.37 CAPID0 bit 30 (DDR2 Frequency Capability):
	 *   0 = all frequencies (DDR2-667 or lower)
	 *   1 = DDR2-533 only
	 * FSB-533 also limits to DDR2-533.
	 */
	unsigned int max_mem_mhz;
	const uint32_t capid0_lo = pci_read_config32(D0F0, 0xe0);
	if ((capid0_lo & (1 << 30)) || si->timings.fsb_clock == FSB_CLOCK_533MHz)
		max_mem_mhz = 266;  /* DDR2-533 */
	else
		max_mem_mhz = 333;  /* DDR2-667 */

	/*
	 * Phase 3: select clock then derive CAS from absolute time.
	 * Try fastest first: DDR2-667 (mc=2), then DDR2-533 (mc=1).
	 */
	unsigned int best_CAS = 0;

	for (int mc = MEM_CLOCK_667MT; mc >= MEM_CLOCK_533MT; mc--) {
		unsigned int clock_mhz;
		switch (mc) {
		case MEM_CLOCK_667MT:
			clock_mhz = 333;
			break;
		case MEM_CLOCK_533MT:
			clock_mhz = 266;
			break;
		default:
			continue;
		}

		if (clock_mhz > max_mem_mhz)
			continue;

		unsigned int tCK_clock = tCK_from_clock[mc];
		if (tCK_clock < tCKmin_common)
			continue;  /* clock faster than any DIMM can sustain */

		/*
		 * Convert the absolute tAA requirement to clock cycles at
		 * this frequency, then find the lowest valid CAS in the
		 * common mask that meets or exceeds that cycle count.
		 */
		unsigned int cas_needed = DIV_ROUND_UP(tAA_needed, tCK_clock);
		if (cas_needed < 3)
			cas_needed = 3;

		for (unsigned int cas = cas_needed; cas <= 6; cas++) {
			if (cas_mask & (1u << cas)) {
				best_CAS = cas;
				break;
			}
		}

		if (best_CAS) {
			si->timings.mem_clock = mc;
			si->timings.CAS = best_CAS;
			break;
		}
	}

	if (!best_CAS)
		die("No valid frequency/CAS combination (POST 0xEC)");

	printk(BIOS_DEBUG, "DRAM: %s CAS%d (tCKmin_common=%u tCK=%u "
	       "tAA_needed=%u cas_mask=0x%02x)\n",
	       si->timings.mem_clock == MEM_CLOCK_667MT ? "DDR2-667" : "DDR2-533",
	       best_CAS, tCKmin_common,
	       tCK_from_clock[si->timings.mem_clock],
	       tAA_needed, cas_mask);
}

/* ================================================================== */
/* Timing parameter calculation                                       */
/* ================================================================== */

/*
 * calculate_timings() - Compute DRAM timing parameters from SPD
 *
 * Converts SPD timing values (in ns or 1/4 ns) to clock cycles
 * at the selected memory frequency. Takes the worst case across
 * all populated DIMMs.
 *
 * BIOS equivalent: FFF022DF / FFFF4B7F (POST 0xFF11-0xFF15)
 * coreboot equivalent: calculate_derived_timings()
 */
static void calculate_timings(sysinfo_t *si)
{
	int slot;
	timings_t *t = &si->timings;
	unsigned int tCK = tCK_from_clock[t->mem_clock];
	unsigned int tRAS_max = 0, tRP_max = 0, tRCD_max = 0;
	unsigned int tWR_max = 0, tRRD_max = 0, tRTP_max = 0;
	unsigned int tRFC_max = 0;

	for (slot = 0; slot < 4; slot++) {
		dimminfo_t *d = &si->dimms[slot];
		if (!d->present)
			continue;

		/*
		 * All timing values are already in 1/256 ns units,
		 * decoded by the common DDR2 SPD library.  No manual
		 * conversion from raw SPD byte encodings is needed.
		 */

		/* tRAS */
		if (DIV_ROUND_UP(d->tRAS, tCK) > tRAS_max)
			tRAS_max = DIV_ROUND_UP(d->tRAS, tCK);

		/* tRP */
		if (DIV_ROUND_UP(d->tRP, tCK) > tRP_max)
			tRP_max = DIV_ROUND_UP(d->tRP, tCK);

		/* tRCD */
		if (DIV_ROUND_UP(d->tRCD, tCK) > tRCD_max)
			tRCD_max = DIV_ROUND_UP(d->tRCD, tCK);

		/* tWR */
		if (DIV_ROUND_UP(d->tWR, tCK) > tWR_max)
			tWR_max = DIV_ROUND_UP(d->tWR, tCK);

		/* tRRD: minimum 2 clocks per JEDEC */
		unsigned int tRRD_clk = DIV_ROUND_UP(d->tRRD, tCK);
		if (tRRD_clk < 2)
			tRRD_clk = 2;
		if (tRRD_clk > tRRD_max)
			tRRD_max = tRRD_clk;

		/* tRTP: minimum 2 clocks per JEDEC */
		unsigned int tRTP_clk = DIV_ROUND_UP(d->tRTP, tCK);
		if (tRTP_clk < 2)
			tRTP_clk = 2;
		if (tRTP_clk > tRTP_max)
			tRTP_max = tRTP_clk;

		/*
		 * tRFC: from lookup table based on chip capacity.
		 * Chip capacity is encoded: 0=256Mb, 1=512Mb, 2=1Gb, 3=2Gb.
		 */
		unsigned int width_bits = (d->x16) ? 4 : 3;
		unsigned int bank_bits = (d->banks == 8) ? 3 : 2;
		int cap_idx = d->rows + d->cols + width_bits + bank_bits - 28;
		if (cap_idx < 0)
			cap_idx = 0;
		if (cap_idx > 3)
			cap_idx = 3;
		unsigned int tRFC = tRFC_table[t->mem_clock - 1][cap_idx];
		if (tRFC > tRFC_max)
			tRFC_max = tRFC;
	}

	/* Bounds checking (matching BIOS fatal codes) */
	if (tRAS_max < 4 || tRAS_max > 31)
		die("tRAS out of range (POST 0xE4)");
	if (tRP_max < 2 || tRP_max > 9)
		die("tRP out of range (POST 0xED)");
	if (tRCD_max < 2 || tRCD_max > 9)
		die("tRCD out of range (POST 0xEE)");
	if (tRFC_max > 255)
		die("tRFC out of range (POST 0xEF)");

	t->tRAS = tRAS_max;
	t->tRP  = tRP_max;
	t->tRCD = tRCD_max;
	t->tWR  = tWR_max;
	t->tRRD = tRRD_max;
	t->tRTP = tRTP_max;
	t->tRFC = tRFC_max;

	printk(BIOS_DEBUG, "Timings: CAS%u tRAS=%u tRP=%u tRCD=%u "
	       "tWR=%u tRFC=%u tRRD=%u tRTP=%u\n",
	       t->CAS, t->tRAS, t->tRP, t->tRCD,
	       t->tWR, t->tRFC, t->tRRD, t->tRTP);
}

/* ================================================================== */
/* Clock configuration programming                                    */
/* ================================================================== */

/*
 * me_pll_handshake() - HECI notification before/after PLL frequency change
 *
 * Notifies the ME that the host is about to change the PLL (or has just
 * finished), giving the ME a chance to quiesce its own clocked logic.
 *
 * Protocol (vendor: FFF0366A / FFF0335C):
 *   1. In H_CSR: assert H_RST, clear H_RDY, trigger H_IG - tells ME a
 *      transition is starting.
 *   2. Poll ME_CSR_HA until ME asserts its RDY bit.
 *   3. If H_RDY is still clear: deassert H_RST, assert H_RDY, trigger H_IG
 *      to acknowledge the ME.
 */
static void me_pll_handshake(volatile uint8_t *d3bar)
{
	uint32_t h_csr;

	/* Step 1: assert reset, clear ready, interrupt ME */
	h_csr = read32(d3bar + ME_H_CSR);
	h_csr = (h_csr & ~ME_CSR_RDY) | ME_CSR_RST | ME_CSR_IG;
	write32(d3bar + ME_H_CSR, h_csr);

	/* Step 2: wait for ME to signal ready */
	while (!(read32(d3bar + ME_ME_CSR_HA) & ME_CSR_RDY))
		;

	/* Step 3: restore host-ready state if needed */
	h_csr = read32(d3bar + ME_H_CSR);
	if (!(h_csr & ME_CSR_RDY)) {
		h_csr = (h_csr & ~ME_CSR_RST) | ME_CSR_RDY | ME_CSR_IG;
		write32(d3bar + ME_H_CSR, h_csr);
	}
}

/*
 * program_clkcfg_lock() - Program DRAM frequency in CLKCFG and relock PLL
 *
 * BIOS equivalent: FFFF4DF7 (121 bytes, POST 0xFF18), plus
 *   FFF009AD (PLL programming, 188 bytes) and
 *   FFF00A8E (DCC / UPDATE toggle, 140 bytes).
 *
 * GM965 CLKCFG memory clock encoding in bits [6:4]:
 *   DDR2-533 (mc=1) -> 3, DDR2-667 (mc=2) -> 4
 *   Formula: (mem_clock + 2) << 4
 *
 * Sequence:
 *   1. Skip PLL relock on warm boot when frequency already matches.
 *   2. On A0 stepping only: ensure bits [9:8] are set in CLKCFG.
 *   3. Map D3:F0 ME BAR to 0xFED10000 and enable bus mastering.
 *      D3:F0 is already visible via DEVEN_D3F0 set in gm965_early_init().
 *      If ME firmware is absent (D3:F0 returns 0xffff) the HECI
 *      handshakes are skipped; the PLL still relocks via hardware alone.
 *   4. Write new mem clock to CLKCFG (UPDATE clear).
 *   5. HECI handshake #1: notify ME of impending frequency change.
 *   6. Perform UPDATE toggle (clear -> set -> clear) to latch frequency.
 *   7. HECI handshake #2: notify ME the frequency change is complete.
 */
static void program_clkcfg_lock(const sysinfo_t *si)
{
	uint32_t clkcfg_orig = mchbar_read32(CLKCFG_MCHBAR);
	uint32_t clkcfg = clkcfg_orig & ~(1 << 17);
	uint32_t want_memclk = (si->timings.mem_clock + 2) << 4;

	printk(BIOS_DEBUG, "%s: CLKCFG orig=0x%08x want_memclk=0x%02x\n",
	       __func__, clkcfg_orig, want_memclk);

	/*
	 * Vendor (FFFF4DF7): A0 stepping only sets bits 9:8.
	 * On C0+ these bits are already set from hardware default.
	 */
	if (northbridge_stepping() == 0x00
	    && (clkcfg_orig & 0x300) != 0x300) {
		clkcfg |= 0x300;
	} else if ((clkcfg_orig & CLKCFG_MEMCLK_MASK) == want_memclk
		   && si->s3_resume) {
		/*
		 * Frequency already matches AND warm boot - skip PLL
		 * relock.  On cold boot the PLL/UPDATE sequence must
		 * run even when the strap default equals the target
		 * frequency (e.g. DDR2-533 on an X61 with CLKCFG
		 * strap 011).
		 */
		printk(BIOS_DEBUG, "%s: CLKCFG freq already set (warm)\n", __func__);
		return;
	}

	/* Map D3:F0 BAR and enable memory + bus-master (vendor: FFF028BA) */
	/* Currently we don't enable ME in early_init.c as this code seems to choke on it */
	const bool me_active = pci_read_config16(D3F0, PCI_DEVICE_ID) != 0xffff;
	volatile uint8_t *d3bar = (volatile uint8_t *)(uintptr_t)D3F0_BAR;
	if (me_active) {
		pci_write_config32(D3F0, PCI_BASE_ADDRESS_0, D3F0_BAR | 1);
		pci_or_config8(D3F0, PCI_COMMAND,
			       PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	}

	/* Write new frequency to CLKCFG (UPDATE clear) */
	clkcfg = (clkcfg & ~(CLKCFG_UPDATE | CLKCFG_MEMCLK_MASK)) | want_memclk;
	mchbar_write32(CLKCFG_MCHBAR, clkcfg);

	/* HECI handshake #1: notify ME before frequency change */
	if (me_active)
		me_pll_handshake(d3bar);

	/*
	 * UPDATE toggle (vendor: FFF00A33-FFF00A46):
	 *   1. Write freq with UPDATE clear
	 *   2. Write freq with UPDATE set  -> triggers HW latch
	 *   3. Write freq with UPDATE clear -> finalize
	 */
	mchbar_clrsetbits32(CLKCFG_MCHBAR,
			    CLKCFG_MEMCLK_MASK | CLKCFG_UPDATE,
			    want_memclk);
	mchbar_clrsetbits32(CLKCFG_MCHBAR,
			    CLKCFG_MEMCLK_MASK,
			    want_memclk | CLKCFG_UPDATE);
	mchbar_clrbits32(CLKCFG_MCHBAR, CLKCFG_UPDATE);

	/* HECI handshake #2: notify ME that frequency change is complete */
	if (me_active)
		me_pll_handshake(d3bar);

	printk(BIOS_DEBUG, "%s: CLKCFG after=0x%08x\n",
	       __func__, mchbar_read32(CLKCFG_MCHBAR));
}

/*
 * program_gcfgc() - Program IGD render/sampler clocks in GCFGC register
 *
 * BIOS equivalent: FFFF3844 (197 bytes, POST 0xFF19)
 * Despite the post code label, this function programs the IGD's GCFGC
 * register (D2:F0 offset 0xF0), NOT the MCHBAR CLKCFG register.
 * CLKCFG is handled by the inline code at POST 0xFF18.
 *
 * HPLLVCO field == 7 is a special case: programs MCHBAR+0x1190/0x119E
 * instead of GCFGC.
 */
static void program_gcfgc(const sysinfo_t *si)
{
	/* bits 44:42 of CAPID0, which are "GFX Software Capability ID" */
	uint8_t hpllvco = pci_read_config8(D0F0, 0xe5);
	uint8_t vco_field = (hpllvco >> 2) & 7;

	if (vco_field == 7) {
		/*
		 * Special case: VCO field 7 means no standard GCFGC programming.
		 * BIOS: if D0F0 revision (offset 0x08) == 0, configure
		 * MCHBAR scheduling registers instead.
		 */
		if (pci_read_config8(D0F0, PCI_REVISION_ID) == 0) {
			mchbar_setbits16(IOSCHED_190, 0x4000);
			mchbar_clrsetbits16(IOSCHED_19E, 0xe000, 0x9000);
		}
		return;
	}

	/* Set MCHBAR_FFC bit 24 for non-special VCO */
	mchbar_setbits32(MCHBAR_FFC, 1 << 24);

	/*
	 * GCFGC render clock encoding (BIOS):
	 *   FSB=800: 800MT->2, 667MT->3, 533MT->4
	 *   FSB=667: always 5
	 *   HPLLVCO_field==3 override: 4
	 *
	 * GCFGC byte 0 bits [3:0] = render clock, byte 1 bits [4:0] = sampler (fixed 2)
	 */
	uint8_t gcfgc_render = 5;  /* default for FSB 667 */

	if (si->timings.fsb_clock == FSB_CLOCK_800MHz) {
		switch (si->timings.mem_clock) {
		case MEM_CLOCK_533MT:
			gcfgc_render = 4;
			break;
		case MEM_CLOCK_667MT:
			gcfgc_render = 3;
			break;
		}
	}

	/* VCO field 3 forces render clock to 4 */
	if (vco_field == 3)
		gcfgc_render = 4;

	/* Write render clock to GCFGC byte 0, sampler to byte 1 */
	pci_update_config8(D2F0, GCFGC_OFFSET, 0xd0, gcfgc_render);
	pci_update_config8(D2F0, GCFGC_OFFSET + 1, 0xe0, 2);
}

/* ================================================================== */
/* IGD memory frequency / clock crossing setup                        */
/* ================================================================== */

/*
 * Clock crossing ROM tables for POST 0xFF20.
 * Table 1 writes to MCHBAR+0x0208/0x020C (CLKCROSS_DATA3/DATA2).
 * Table 2 writes to MCHBAR+0x1258/0x125C and 0x1358/0x135C (per-channel crossing).
 * Indexed by [mem_clock][fsb_clock], 2 DWORDs per entry.
 *
 * Extracted from BIOS ROM at file offsets 0x175868 and 0x1757E8.
 * ROM index formula: mc + fsb * 4 (NOT fsb + mc * 4).
 * Each entry is 2 DWORDs; stride 4 per FSB row, 1 per mc column.
 */
static const uint32_t clkcross_table1[][4][3] = {
	[MEM_CLOCK_533MT] = {
		[FSB_CLOCK_533MHz] = { 0x000300c0, 0x0030000c },  /* vendor ROM idx=5  (1:1 rate) */
		[FSB_CLOCK_667MHz] = { 0x00070300, 0x00e00018 },  /* vendor ROM idx=13 */
		[FSB_CLOCK_800MHz] = { 0x00070e00, 0x01c00038 },  /* vendor ROM idx=9  + inteltool */
	},
	[MEM_CLOCK_667MT] = {
		[FSB_CLOCK_533MHz] = { 0x00000000, 0x00000000 },  /* vendor ROM idx=6  (zeros, N/A on X61) */
		[FSB_CLOCK_667MHz] = { 0x000300c0, 0x0030000c },  /* vendor ROM idx=14 (1:1 rate) */
		[FSB_CLOCK_800MHz] = { 0x00030e00, 0x0070000c },  /* vendor ROM idx=10 + inteltool */
	},
};

static const uint32_t clkcross_table2[][4][3] = {
	[MEM_CLOCK_533MT] = {
		[FSB_CLOCK_533MHz] = { 0x00100401, 0x00000000 },  /* vendor ROM idx=5  (1:1 rate) */
		[FSB_CLOCK_667MHz] = { 0x10080201, 0x00000040 },  /* vendor ROM idx=13 */
		[FSB_CLOCK_800MHz] = { 0x00020108, 0x00000000 },  /* vendor ROM idx=9  + inteltool */
	},
	[MEM_CLOCK_667MT] = {
		[FSB_CLOCK_533MHz] = { 0x00000000, 0x00000000 },  /* vendor ROM idx=6  (zeros, N/A on X61) */
		[FSB_CLOCK_667MHz] = { 0x00100401, 0x00000000 },  /* vendor ROM idx=14 (1:1 rate) */
		[FSB_CLOCK_800MHz] = { 0x00040210, 0x00000000 },  /* vendor ROM idx=10 + inteltool */
	},
};

/*
 * set_clkcross_frequencies() - Program clock crossing timings
 *
 * BIOS equivalent: FFFF392E (140 bytes, POST 0xFF20)
 * Writes clock crossing values from ROM tables into MCHBAR registers.
 * Also handles 667MT+FSB667 special case (MCHBAR+0x0210 = 0x180).
 */
static void set_clkcross_frequencies(const sysinfo_t *si)
{
	unsigned int mc = si->timings.mem_clock;
	unsigned int fsb = si->timings.fsb_clock;

	/* Write clock crossing data to MCHBAR+0x0208/0x020C */
	mchbar_write32(CLKCROSS_DATA3, clkcross_table1[mc][fsb][0]);
	mchbar_write32(CLKCROSS_DATA2, clkcross_table1[mc][fsb][1]);

	/* Special case: 667MT + FSB800 -> MCHBAR+0x0210 = 0x180 */
	if (mc == MEM_CLOCK_667MT && fsb == FSB_CLOCK_800MHz)
		mchbar_write32(CLKCROSS_DATA1, 0x180);

	/*
	 * Per-channel clock crossing registers at MCHBAR+0x1258/0x125C
	 * and 0x1358/0x135C.
	 * Vendor (FFFF392E) writes to 0xfed15258 = MCHBAR+0x1258.
	 * Verified: inteltool 0x1258 = 0x00040210 matches table value.
	 */
	int ch;
	for (ch = 0; ch < 2; ch++) {
		mchbar_write32(0x1258 + ch * 0x100, clkcross_table2[mc][fsb][0]);
		mchbar_write32(0x125c + ch * 0x100, clkcross_table2[mc][fsb][1]);
	}
}

/* ================================================================== */
/* Pre-JEDEC memory map (temporary)                                   */
/* ================================================================== */

/*
 * prejedec_memory_map() - Set up temporary memory map for JEDEC init
 *
 * Programs CxDRBy with 128MB per rank (dummy) so JEDEC commands can
 * be addressed to each rank.
 *
 * BIOS equivalent: FFFF3EC7 with param=1 (POST 0xFF22)
 * coreboot equivalent: prejedec_memory_map()
 */
static void prejedec_memory_map(sysinfo_t *si)
{
	int ch, s;
	/*
	 * Vendor (FFF01278 with param=1) iterates 4 DIMM slots in a
	 * single loop.  Per slot it interleaves 16-bit DRB writes with
	 * 8-bit DRA writes in a specific order:
	 *
	 *   write16 DRB[rank_even]          (rank 0 or 2 boundary)
	 *   if dual_rank:
	 *       write8  DRA[slot]           (full DRA value)
	 *       boundary += rank_size       (advance for rank 1/3)
	 *   else:
	 *       dra &= 0x0F                 (mask to lower nibble)
	 *   write8  DRA[slot]               (always, 2nd write for dual)
	 *   write16 DRB[rank_odd]           (rank 1 or 3 carry-forward)
	 *
	 * Boundary accumulates across both channels; reset to 0 only
	 * when ch1 has DIMMs (each channel gets its own address space).
	 *
	 * rank_size = 4 << banks_shift  (ROM base=4 at FFF038B4)
	 * dra_base  = 0x22              (ROM value at FFF038B8)
	 */
	uint8_t boundary = 0;

	for (ch = 0; ch < 2; ch++) {
		/*
		 * RAMINIT (FFF0133E) resets boundary for ch1 because its
		 * JEDEC init uses I/O port commands (no address targeting).
		 *
		 * RAMINIT3 (FFFF3EC7) does NOT reset when ch1 is populated
		 * and it's pre-JEDEC, because its JEDEC init (FFFF4573) uses
		 * DCC + memory-read rank targeting - the read address must
		 * fall in the correct rank's global address range.
		 *
		 * Our jedec_init_ddr2() uses the DCC approach (like RAMINIT3),
		 * so ch1 DRBs must be global cumulative for get_rank_addr()
		 * to produce correct addresses.  Without this, ch1 rank 1
		 * would be addressed at DRB(1,0)<<25 = 128MB, targeting
		 * ch0 rank 1 instead of ch1 rank 1 at 384MB.
		 */

		for (s = 0; s < 2; s++) {
			int slot = ch * 2 + s;
			uint8_t rank_size = 0;
			uint8_t dra = 0;

			if (slot < 4 && si->dimms[slot].present) {
				/*
				 * Vendor (FFF01282) always uses the first ROM table
				 * entry (rank_size=4, dra=0x22) for pre-JEDEC,
				 * regardless of bank count.  Each rank gets 128 MB
				 * of address space, which is enough for JEDEC MRS
				 * targeting via DRB-based rank decode.
				 */
				rank_size = 4;
				dra = 0x22;  /* ROM table at FFF037D8 */
			}

			/* Step 1: write16 DRB rank_even boundary */
			boundary += rank_size;
			mchbar_write16(CxDRBy_MCHBAR(ch, s * 2), boundary);

			/*
			 * Steps 2-3: DRA writes.
			 *
			 * Dual-rank (FFF012FF-01308): write DRA, advance
			 * boundary, then write DRA again at the common
			 * exit (FFF01319).
			 *
			 * Single-rank (FFF0130F): mask DRA to lower nibble,
			 * then write once at FFF01319.
			 *
			 * Empty: DRA=0, rank_size=0, one write of 0.
			 */
			if (slot < 4 && si->dimms[slot].present
			    && si->dimms[slot].dual_rank) {
				mchbar_write8(CxDRA_MCHBAR(ch) + s, dra);
				boundary += rank_size;
			} else {
				dra &= 0x0f;
			}
			mchbar_write8(CxDRA_MCHBAR(ch) + s, dra);

			/* Step 4: write16 DRB rank_odd carry-forward */
			mchbar_write16(CxDRBy_MCHBAR(ch, s * 2) + 2, boundary);
		}
	}

	/*
	 * DCC2: channel address decode (FFF01378).
	 *
	 * Write 0 here; the final value is derived from EPD_2E[4:0]
	 * after program_epd() runs at POST FF41.  The vendor also
	 * writes 0 at this point (EPD not yet programmed on cold boot).
	 */
	mchbar_write16(DCC2_MCHBAR, 0);

	/*
	 * TOM / TOLUD / TOUUD - vendor FFF0139F-FFF013B2.
	 *
	 * The vendor writes these PCI registers in the SAME function
	 * that programs DRBs, both for pre-JEDEC and final memory map.
	 * Without them, TOLUD stays at its reset default (1 MB), and
	 * the system agent routes all DRAM reads above 1 MB to MMIO
	 * instead of the memory controller.  JEDEC MRS commands for
	 * rank 1 (at 128 MB) and all channel 1 ranks never reach the
	 * MC, so only rank 0 channel 0 gets initialized.
	 *
	 * Pre-JEDEC total = boundary * 32 MB.  With rank_size = 4
	 * (128 MB per rank), a 2-DIMM single-rank system has
	 * boundary = 8, total = 256 MB, matching the vendor trace:
	 *   TOM   (0xA0) = 0x0002  (256 >> 7)
	 *   TOLUD (0xB0) = 0x1000  (256 << 4)
	 *   TOUUD (0xA2) = 0x0100  (256)
	 */
	{
		uint32_t total_mb = (uint32_t)boundary * 32;
		uint32_t tolud_mb = (total_mb < 3072) ? total_mb : 3072;

		pci_write_config16(D0F0, D0F0_TOM, (total_mb >> 7) & 0x1ff);
		pci_write_config16(D0F0, D0F0_TOLUD, tolud_mb << 4);
		pci_write_config16(D0F0, D0F0_TOUUD, total_mb);
	}

	/* Disable channel XOR during pre-JEDEC */
	mchbar_setbits32(DCC_MCHBAR, DCC_NO_CHANXOR);
}

/* ================================================================== */
/* Final memory map programming                                       */
/* ================================================================== */

/*
 * program_memory_map() - Set up final memory map with real sizes
 *
 * Programs CxDRBy with actual rank sizes, sets TOM/TOLUD/TOUUD.
 *
 * BIOS equivalent: FFFF3EC7 with param=0 (POST 0xFF35)
 * coreboot equivalent: program_memory_map()
 */
static void program_memory_map(sysinfo_t *si, u16 ggc)
{
	int ch, s;
	uint32_t total_mb = 0;

	/*
	 * Vendor (FFF01278 with param=0) uses the same per-slot loop
	 * structure as pre-JEDEC but with real rank sizes from the
	 * ROM table at FFF038B4:
	 *   rank_size = ROM[cols_idx + cap_idx] << banks_shift
	 *   dra       = ROM[cols_idx] at FFF038B8  (0x22/0x33/0x44)
	 *
	 * For the final map, boundary resets to 0 per-channel
	 * (vendor: FFF0133E-01343 saves ch0, resets for ch1).
	 */
	for (ch = 0; ch < 2; ch++) {
		uint8_t boundary = 0;

		for (s = 0; s < 2; s++) {
			int slot = ch * 2 + s;
			uint8_t rank_size = 0;
			uint8_t dra = 0;

			if (slot < 4 && si->dimms[slot].present) {
				/*
				 * Rank size in 32 MB units from SPD geometry.
				 * Vendor ROM table at FFF038B4:
				 *   index 0 (9 cols):  4  (128 MB)
				 *   index 1 (10 cols): 8  (256 MB)
				 *   index 2 (11 cols): 16 (512 MB)
				 * Then shifted by banks_shift (8-bank -> +1).
				 */
				int banks_shift = (si->dimms[slot].banks == 8) ? 1 : 0;
				rank_size = (si->dimms[slot].rank_capacity_mb / 32);
				if (rank_size == 0)
					rank_size = 1;
				/*
				 * banks_shift is already baked into
				 * rank_capacity_mb from SPD, so no shift
				 * needed here (unlike pre-JEDEC).
				 */
				(void)banks_shift;

				/*
				 * DRA encoding from ROM table at FFF038B8:
				 *   cols 9  -> 0x22, cols 10 -> 0x33, cols 11 -> 0x44
				 * Per-rank nibble = (cols - 7).  Both nibbles
				 * set for dual-rank.
				 */
				uint8_t nibble = si->dimms[slot].cols - 7;
				dra = nibble | (nibble << 4);

				total_mb += si->dimms[slot].rank_capacity_mb;
			}

			/* Step 1: write16 DRB rank_even boundary */
			boundary += rank_size;
			mchbar_write16(CxDRBy_MCHBAR(ch, s * 2), boundary);

			/*
			 * Steps 2-3: DRA writes, interleaved per vendor
			 * (FFF01308 + FFF01319).
			 *
			 * Dual-rank: write DRA (full), advance boundary
			 *            for rank 1, write DRA again.
			 * Single-rank: mask DRA lower nibble, write once.
			 * Empty: dra=0, one write.
			 */
			if (slot < 4 && si->dimms[slot].present
			    && si->dimms[slot].dual_rank) {
				mchbar_write8(CxDRA_MCHBAR(ch) + s, dra);
				boundary += rank_size;
				total_mb += si->dimms[slot].rank_capacity_mb;
			} else {
				dra &= 0x0f;
			}
			mchbar_write8(CxDRA_MCHBAR(ch) + s, dra);

			/* Step 4: write16 DRB rank_odd carry-forward */
			mchbar_write16(CxDRBy_MCHBAR(ch, s * 2) + 2, boundary);
		}
	}

	/*
	 * DCC2: channel address decode (FFF01378).
	 *
	 * Write 0 here; the final value is derived from EPD_2E[4:0]
	 * after program_epd() runs at POST FF41.  The vendor also
	 * writes 0 at this point (EPD not yet programmed on cold boot).
	 */
	mchbar_write16(DCC2_MCHBAR, 0);

	/*
	 * Program TOM, TOLUD, TOUUD, and REMAP in D0:F0 PCI config.
	 *
	 * Vendor BIOS (D630: FUN_fffec42e, X61: FFF01278/FFFF3EC7):
	 *
	 *   total  = sum of all rank capacities (MB)
	 *   tolud  = min(total, 3584MB)   [MMIO window starts at 3.5 GB]
	 *
	 * When total > tolud + 64 MB, the DRAM above TOLUD is hidden by
	 * the MMIO aperture.  The MCH remap engine re-maps that hidden
	 * DRAM into the region starting at REMAPBASE (above 4 GB), making
	 * it accessible again.
	 *
	 * Vendor algorithm (register units are MB throughout):
	 *   total_aligned = total & ~63           (64 MB alignment)
	 *   remapbase     = max(total_aligned, 4096)
	 *   touud_capped  = min(total_aligned, 4096)
	 *   remaplimit    = remapbase + (touud_capped - tolud) - 64
	 *   touud_final   = remaplimit + 64
	 *
	 * Register encodings (PCI config D0:F0):
	 *   TOM       (0xA0): total >> 7         (128 MB units)
	 *   TOLUD     (0xB0): tolud << 4         (1 MB units, bits [14:4])
	 *   TOUUD     (0xA2): touud              (1 MB units)
	 *   REMAPBASE (0x98): remapbase >> 6     (64 MB units)
	 *   REMAPLIMIT(0x9A): remaplimit >> 6    (64 MB units)
	 */
	si->tom_mb = total_mb;
	si->tolud_mb = (total_mb < 3072) ? total_mb : 3072; /* cap at 3 GB for MMIO */

	uint32_t touud_mb = total_mb;
	int needs_remap = (total_mb - si->tolud_mb) > 64;

	if (needs_remap) {
		/*
		 * Remap the DRAM hidden behind the MMIO aperture to above
		 * 4 GB (or above total if total > 4 GB).
		 *
		 * GM965 REMAPBASE and REMAPLIMIT registers (D0F0 offsets
		 * 0x98 and 0x9A) encode Address[35:26] in bits [9:0], but
		 * require bit 0 (address bit 26) to be 0.  This gives
		 * 128 MB granularity, unlike GM45 which allows 64 MB.
		 *
		 * The vendor BIOS handles this by rounding REMAPLIMIT
		 * down to the nearest even value and setting TOUUD to
		 * match the actual (smaller) remap window.  Up to 64 MB
		 * of hidden DRAM may be lost.
		 *
		 * Vendor inteltool (2x2GB):
		 *   REMAPBASE  = 0x0040 (4096 MB, bit 0 = 0)
		 *   REMAPLIMIT = 0x004E (4992 MB, bit 0 = 0)
		 *   TOUUD      = 0x13C0 (5056 MB = (78+1)*64)
		 *   TOLUD      = 0xC000 (3072 MB, unchanged)
		 */
		uint32_t total_aligned = total_mb & ~63u;
		uint32_t remapbase  = (total_aligned > 4096) ? total_aligned : 4096;
		uint32_t touud_cap  = (total_aligned < 4096) ? total_aligned : 4096;
		uint32_t remaplimit = remapbase + (touud_cap - si->tolud_mb) - 64;

		/* Round register values to even (clear bit 0) for 128 MB alignment */
		uint16_t remapbase_reg  = (uint16_t)((remapbase >> 6) & 0x03fe);
		uint16_t remaplimit_reg = (uint16_t)((remaplimit >> 6) & 0x03fe);

		/* TOUUD must match the actual remap window, not the theoretical size */
		touud_mb = (uint32_t)(remaplimit_reg + 1) * 64;

		pci_write_config16(D0F0, D0F0_REMAPBASE, remapbase_reg);
		pci_write_config16(D0F0, D0F0_REMAPLIMIT, remaplimit_reg);

		printk(BIOS_DEBUG, "memmap: remap enabled - "
		       "REMAPBASE=0x%04x (%uMB) REMAPLIMIT=0x%04x (%uMB) "
		       "TOUUD=%uMB\n",
		       remapbase_reg, remapbase,
		       remaplimit_reg, remaplimit, touud_mb);
	}

	printk(BIOS_DEBUG, "memmap: total=%uMB TOM=%uMB TOLUD=%uMB "
	       "TOUUD=%uMB remap=%d DRA0=0x%08x DRA1=0x%08x\n",
	       total_mb, total_mb, si->tolud_mb, touud_mb, needs_remap,
	       mchbar_read32(CxDRA_MCHBAR(0)),
	       mchbar_read32(CxDRA_MCHBAR(1)));

	pci_write_config16(D0F0, D0F0_TOM, (total_mb >> 7) & 0x1ff);
	pci_write_config16(D0F0, D0F0_TOLUD, si->tolud_mb << 4);
	pci_write_config16(D0F0, D0F0_TOUUD, touud_mb);

	printk(BIOS_DEBUG, "memmap regs: TOM=0x%04x TOLUD=0x%04x TOUUD=0x%04x"
	       " REMAPBASE=0x%04x REMAPLIMIT=0x%04x ESMRAMC=0x%02x\n",
	       pci_read_config16(D0F0, D0F0_TOM),
	       pci_read_config16(D0F0, D0F0_TOLUD),
	       pci_read_config16(D0F0, D0F0_TOUUD),
	       pci_read_config16(D0F0, D0F0_REMAPBASE),
	       pci_read_config16(D0F0, D0F0_REMAPLIMIT),
	       pci_read_config8(D0F0, D0F0_ESMRAMC));

	/*
	 * Program the GGC (Graphics Memory Control) register.
	 *
	 * Writing GGC causes the hardware to compute BSM (Base of Stolen
	 * Memory) as: BSM = TOLUD - stolen_size.  The IGD stolen region
	 * then occupies [BSM, TOLUD).  TSEG is placed just below BSM by
	 * northbridge_get_tseg_base() in memmap.c.
	 *
	 * ggc is returned by igd_compute_ggc() from the gfx_uma_size option.
	 * If bit 1 (IVD) is set, no memory is stolen and IGD is disabled.
	 */
	pci_write_config16(D0F0, D0F0_GGC, ggc);

	/*
	 * Enable TSEG (2 MB) - required for SMM and cbmem.
	 *
	 * ESMRAMC register (D0:F0 offset 0x9E):
	 *   bit 0: T_EN (TSEG enable)
	 *   bits [2:1]: TSEG_SZ (00=1M, 01=2M, 10=8M)
	 *
	 * GM45 reference: pci_update_config8(D0F0, D0F0_ESMRAMC,
	 *                                    ~0x07, (1<<1)|(1<<0));
	 * Sets T_EN=1, TSEG_SZ=01 (2MB).
	 */
	pci_update_config8(D0F0, D0F0_ESMRAMC, ~0x07, (1 << 1) | (1 << 0));

	/*
	 * DCC channel mode is handled by post_jedec_sequence()
	 * (POST 0xFF34) which clears/sets DCC_INTERLEAVED (bit 1)
	 * and sets bit 10 (normal operation).
	 *
	 * The vendor's final memory map (FFF01278 / FFFF3EC7 with
	 * param=0) does NOT modify DCC at all - bit 10 stays set
	 * from post_jedec and must NOT be cleared.
	 */
}

/* ================================================================== */
/* DRAM timing register programming                                   */
/* ================================================================== */

/*
 * program_timings() - Write timing parameters to CxDRT0-CxDRT6
 *
 * Programs both channels with computed timing values.
 *
 * BIOS equivalent: FFF02A7A (1603 bytes) / FFFF408F (881 bytes, POST 0xFF25)
 * coreboot equivalent: dram_program_timings()
 */
static void program_timings(sysinfo_t *si)
{
	timings_t *t = &si->timings;
	int ch;

	/*
	 * Compute tRRD adjustment for CxDRT1 bits [12:10].
	 *
	 * Vendor logic (FFF0146B decompiled):
	 *   mc==1 (800MT): default 0, scan DIMMs:
	 *     if (present && cols > 9 && x16) -> trrd_adj = 1
	 *   mc==2 (667MT): default 1, scan DIMMs:
	 *     if (present && ((cols > 9 && x16) || cols > 10)) -> trrd_adj = 2
	 *
	 * This is a geometry-based check on page size / DIMM width,
	 * NOT a dual-rank check.  Standard x8 DDR2 with 10 columns
	 * keeps the default.  Only x16 DIMMs or those with >10 cols
	 * (e.g., 2KB page) bump the tRRD value up.
	 *
	 * Maps to CxDRT1 tRRD bits [12:10].
	 *
	 * Vendor struct offsets:
	 *   puVar7[-6] = present (byte 0), puVar7[-4] = x16 (byte 2),
	 *   *puVar7 = cols_minus_9 (byte 6)
	 */
	int trrd_adj = 0;
	if (t->mem_clock == MEM_CLOCK_533MT) {
		for (int s = 0; s < 4; s++) {
			if (si->dimms[s].present
			    && (si->dimms[s].cols > 9)
			    && si->dimms[s].x16)
				trrd_adj = 1;
		}
	} else if (t->mem_clock == MEM_CLOCK_667MT) {
		trrd_adj = 1;
		for (int s = 0; s < 4; s++) {
			if (si->dimms[s].present
			    && (((si->dimms[s].cols > 9) && si->dimms[s].x16)
				|| (si->dimms[s].cols > 10)))
				trrd_adj = 2;
		}
	}

	for (ch = 0; ch < 2; ch++) {
		uint32_t reg;

		/*
		 * --- CxDRT0 (EDS 4.7.5) ---
		 *
		 * Two-step programming:
		 * 1) Lower 18 bits: from ROM base table per mem_clock.
		 * 2) Upper word BtB fields (EDS formulas, DDR2 BL=8):
		 *    [30:26] B2BWR2PCSB = WL + BL/2 + tWR = (CAS-1) + 4 + tWR
		 *    [23:20] B2BWR2RDSR = WL + BL/2 + tWTR = (CAS-1) + 4 + tWTR
		 * Verified: inteltool DRT0 = 0x34b10841, WtP=13, WtR=11.
		 */
		reg = mchbar_read32(CxDRT0_MCHBAR(ch));

		/* Step 1: Apply ROM table to lower fields */
		/*
		 * CxDRT0 base values (ROM table at BIOS file offset 0x175A78).
		 * Indexed by mem_clock (1-based). Applied with mask 0xfffc4318.
		 * Sets fields: bits [17:15], [13:10], [7:5], [2:0].
		 * Values are the same for both supported freq so use a const.
		 */
		const uint32_t drt0_base = 0x00010841;

		reg = (reg & 0xfffc4318) | drt0_base;

		/* Step 2: Compute BtB values (tWL = CAS-1, BL/2 = 4) */
		int btb_wtp = (t->CAS - 1) + 4 + t->tWR;
		int btb_wtr = (t->CAS - 1) + 4 + drt0_twtr_lut[t->mem_clock];

		reg = (reg & ~(0xfU << 20)) | ((btb_wtr & 0xf) << 20);
		reg = (reg & ~(0x1fU << 26)) | ((btb_wtp & 0x1f) << 26);
		mchbar_write32(CxDRT0_MCHBAR(ch), reg);

		/*
		 * --- CxDRT1 (EDS 4.7.6) ---
		 *
		 * Preserve mask 0xcc1fe318 (bits 31:30,27:26,20:19,17:16,
		 *   13,9:8,4:3 are reserved/preserved).
		 *   [29:28] = tRTP  (00=BL/2 for DDR2-533, 01=BL/2+1 for DDR2-667)
		 *   [25:21] = tRAS  (Activate to Precharge delay)
		 *   [18]    = Precharge to Precharge Delay (keep at 1 clock reset default)
		 *   [15]    = tRPALL (Pre-All to Activate, preserved from odt_pre_setup)
		 *   [12:10] = tRRD  (Activate to Activate delay)
		 *   [7:5]   = tRCD - 2 (DRAM RASB to CASB Delay)
		 *   [2:0]   = tRP  - 2 (DRAM RASB Precharge)
		 */
		{
			uint32_t drt1_val = (t->tRAS << 21)
					  | ((t->tRCD - 2) << 5)
					  | (t->tRP - 2);
			if (t->mem_clock == MEM_CLOCK_667MT)
				drt1_val |= (1 << 28);
			drt1_val |= (trrd_adj << 10);

			reg = mchbar_read32(CxDRT1_MCHBAR(ch));
			reg = (reg & 0xcc1fe318) | drt1_val;
			mchbar_write32(CxDRT1_MCHBAR(ch), reg);
		}

		/*
		 * --- CxDRT2 (EDS 4.7.7) ---
		 *
		 *   [21:17] = tFAW (Rolling Activate Window)
		 *   [4:0]   = 0x10 (fixed)
		 */
		{
			const uint32_t tFAW = tFAW_fixed[t->mem_clock];
			uint32_t drt2 = mchbar_read32(CxDRT2_MCHBAR(ch));
			drt2 = (drt2 & ~(0x1f <<  0)) | (0x10 <<  0);
			drt2 = (drt2 & ~(0x1f << 17)) | (tFAW << 17);
			mchbar_write32(CxDRT2_MCHBAR(ch), drt2);
		}

		/*
		 * --- CxDRT3 (EDS 4.7.8) ---
		 *
		 *   [25:23] = CASB Latency (CAS - 3)
		 *   [20:13] = Refresh Cycle Time (tRFC)
		 *   [2:0]   = Write Latency (tWL - 2, DDR2: tWL = CAS - 1)
		 * Verified: inteltool DRT3 = 0x01056282, CAS=5, tRFC=43.
		 */
		{
			const uint32_t tWL = t->CAS - 1;
			uint32_t drt3 = mchbar_read32(CxDRT3_MCHBAR(ch));
			drt3 = (drt3 & ~(0x07 << 23)) | ((t->CAS - 3) << 23);
			drt3 = (drt3 & ~(0xff << 13)) | (t->tRFC << 13);
			drt3 = (drt3 & ~(0x07 <<  0)) | ((tWL - 2) <<  0);
			mchbar_write32(CxDRT3_MCHBAR(ch), drt3);
		}

		/*
		 * --- CxDRT4 ---
		 *
		 * BIOS: load full register from ROM table.
		 */
		mchbar_write32(CxDRT4_MCHBAR(ch), drt4_rom_table[t->mem_clock]);

		/*
		 * --- CxDRT5 ---
		 *
		 * EDS 4.7.10:
		 *   bits [25:22] = TS Read Delay (BL/2 + CL + 2)
		 *   bits [20:12] = Read Slave DLL Lock Timer (ROM table)
		 *   bits [2:1]   = DQ/DQS Sense Amp Duration
		 * Verified: inteltool DRT5 = 0x62c64042, bits[25:22]=11.
		 */
		{
			const uint32_t ts_read_delay = 4 + t->CAS + 2; /* BL/2 + CL + 2 */
			uint32_t drt5 = mchbar_read32(CxDRT5_MCHBAR(ch));
			drt5 = (drt5 & ~(0x00f << 22)) | (ts_read_delay << 22);
			drt5 = (drt5 & ~(0x1ff << 12)) | (drt5_rom_bytes[t->mem_clock] << 12);
			drt5 = (drt5 & ~(0x003 <<  1)) | (1 << 1);
			mchbar_write32(CxDRT5_MCHBAR(ch), drt5);
		}

		/* --- CxDRT6: ZQ bit clear for DDR2 --- */
		reg = mchbar_read32(CxDRT6_MCHBAR(ch));
		reg &= ~(1 << 2);  /* DDR2: no ZQ cal */
		mchbar_write32(CxDRT6_MCHBAR(ch), reg);
	}
}

/* ================================================================== */
/* Rank enable and DRAM control registers                             */
/* ================================================================== */

/*
 * program_dram_control() - Set CxDRC0/CxDRC1/CxDRC2 for each channel
 *
 * BIOS equivalent: FFFF408F (tail section, lines 1750-1771)
 *
 * CxDRC0: bits [10:8] from param[0x20] (rank config byte), bit 3 set
 * CxDRC1: rank not-populated bits [17:16], plus constant |= 0xc1800
 * CxDRC2: rank disable bits [25:24], plus constant |= 0xc001000
 */
static void program_dram_control(sysinfo_t *si)
{
	int ch;

	for (ch = 0; ch < 2; ch++) {
		int slot = ch * 2;
		uint32_t reg;

		/*
		 * CxDRC0 (EDS 4.7.11): DRAM Controller Mode 0
		 *   [10:8] = RMS: Refresh Mode Select (7.8us = 2)
		 *   [3]    = BL: Burst Length 8
		 * Verified: inteltool CxDRC0 = 0x4000020a, bits[10:8] = 2.
		 */
		reg = mchbar_read32(CxDRC0_MCHBAR(ch));
		reg = (reg & ~CxDRC0_RMS_MASK) | CxDRC0_RMS_78US;
		reg |= (1 << 3); /* Burst length = 8 */
		mchbar_write32(CxDRC0_MCHBAR(ch), reg);

		/*
		 * CxDRC1 (EDS 4.7.12): DRAM Controller Mode 1
		 *   [19:16] = CKE tri-state per rank (set for unpopulated)
		 *   [12]    = CSBTRIEN: CS# tri-state enable
		 *   [11]    = ADRTRIEN: address tri-state enable
		 */
		reg = mchbar_read32(CxDRC1_MCHBAR(ch));
		if (!si->dimms[slot].present) {
			/* No DIMM: tri-state both ranks' CKE */
			reg |= CxDRC1_CKE_TRISTATE(0) | CxDRC1_CKE_TRISTATE(1);
		} else if (!si->dimms[slot].dual_rank) {
			/* Single rank: tri-state rank 1's CKE */
			reg |= CxDRC1_CKE_TRISTATE(1);
		}
		reg |= 0xc0000 | CxDRC1_CSBTRIEN | CxDRC1_ADRTRIEN;
		mchbar_write32(CxDRC1_MCHBAR(ch), reg);

		/*
		 * CxDRC2 (EDS 4.7.13): DRAM Controller Mode 2
		 *   [27:24] = ODT tri-state per rank (set for unpopulated)
		 *   [13]    = Clock Control to DQ Buffers
		 *   [12]    = constant
		 */
		reg = mchbar_read32(CxDRC2_MCHBAR(ch));
		if (!si->dimms[slot].present) {
			/* No DIMM: tri-state both ranks' ODT */
			reg |= 0x3000000;
		} else if (!si->dimms[slot].dual_rank) {
			/* Single rank: tri-state rank 1's ODT */
			reg |= 0x2000000;
		}
		reg |= 0xc001000;
		mchbar_write32(CxDRC2_MCHBAR(ch), reg);
	}
}

/* ================================================================== */
/* RCOMP Initialization                                               */
/* ================================================================== */

/*
 * RCOMP ROM table data - extracted from X61 BIOS at 0xFFFF5910
 * 9 groups x 10 DWORDs (40 bytes) per group = 360 bytes total.
 * Copied to MCHBAR RCOMP_TABLES (0x0680) with gaps.
 *
 * Layout per group in ROM: [6 DWORDs (pull-up/down base)] [4 DWORDs (strength)]
 * Layout per group in MCHBAR: 64 bytes with gaps after entries 2 and 5.
 */
static const uint32_t rcomp_rom_table[9][10] = {
	{	/* Group 0 */
		0x4c28a249, 0xe38e34d3, 0x3cf3cf38,
		0x4c2ca249, 0xe38e34d3, 0x3cf3cf3c,
		0x00000055, 0x55000000, 0x00000000, 0x00000000
	},
	{	/* Group 1 */
		0xc8186145, 0xc30c2cb2, 0x34d34d30,
		0x481c71c6, 0xb2ca28a2, 0x30c30c30,
		0x00000055, 0x55000000, 0x00000000, 0x00000000
	},
	{	/* Group 2 */
		0xc8186145, 0xc30c2cb2, 0x34d34d30,
		0x481c71c6, 0xb2ca28a2, 0x30c30c30,
		0x00000055, 0x55000000, 0x00000000, 0x80000000
	},
	{	/* Group 3 */
		0xc8186145, 0xc30c2cb2, 0x34d34d30,
		0x481c71c6, 0xb2ca28a2, 0x30c30c30,
		0x00000055, 0x55000000, 0x00000000, 0x80000000
	},
	{	/* Group 4 */
		0xca28a249, 0x24903cb2, 0x4d34d349,
		0xcd34d30c, 0x349140f3, 0x5d759655,
		0x00000088, 0x88000000, 0x00000000, 0x00000000
	},
	{	/* Group 5 */
		0xca28a249, 0x24903cb2, 0x4d34d349,
		0xcd34d30c, 0x349140f3, 0x5d759655,
		0x00000088, 0x88000000, 0x00000000, 0x00000000
	},
	{	/* Group 6 */
		0xca28a249, 0x24903cb2, 0x4d34d349,
		0xca28a249, 0x140e34b2, 0x4d349245,
		0x00000088, 0x88000000, 0x00000000, 0x00000000
	},
	{	/* Group 7 */
		0x4c28a249, 0xe38e34d3, 0x3cf3cf38,
		0x4c2ca249, 0xe38e34d3, 0x3cf3cf3c,
		0x00000055, 0x55000000, 0x00000000, 0x00000000
	},
	{	/* Group 8 */
		0xc8186145, 0xc30c2cb2, 0x34d34d30,
		0x481c71c6, 0xb2ca28a2, 0x30c30c30,
		0x00000055, 0x55000000, 0x00000000, 0x00000000
	},
};

/*
 * rcomp_init() - Initialize RCOMP calibration engine
 *
 * Programs RCOMP registers, copies ROM compensation tables to MCHBAR,
 * and starts initial calibration cycle.
 *
 * BIOS equivalent: FFFF3C49 (296 bytes, POST 0xFF23)
 * coreboot equivalent: rcomp_initialization() (DDR2 path)
 *
 * Key differences from original code (audit findings):
 *   - RCOMP_STATUS must be: & 0x8888 | 0x1111
 *   - RCOMP_CFG must be: & 0xc1ff | 0x2e00
 *   - RCOMP_CFG4 must be: & 0x99999999 | 0x11119999
 *   - RCOMP_ODT0/1 must be: & 0xc0 | 0x36 (not 0x09)
 *   - ROM table must be copied to RCOMP_TABLES
 */
static void rcomp_init(sysinfo_t *si)
{
	int g, i;

	/*
	 * Enable RCOMP engine clock before any RCOMP_CTRL writes.
	 * Vendor BIOS (fff00ff0): first action at POST ff23 is to set bit 12
	 * of MCHBAR[0x1444].  Without this the RCOMP engine has no clock and
	 * the RCOMP_CTRL bit 0 (busy/GO) never clears.
	 * Cleared again at POST ff40 after all training is complete.
	 */
	mchbar_setbits32(IO_RCOMP_CLK_EN, 1 << 12);

	/*
	 * Step 1: Program RCOMP_CTRL - clear bits 18,4,0; set bits 17,5.
	 * Bit 0 (RCOMP_GO) is cleared explicitly here so that a stale value
	 * from a previous boot cycle (warm reset) does not confuse the engine.
	 * On non-A0 steppings (vendor: DAT_f0000008 != 0), also set bit 18.
	 * The vendor first writes with bits 17,5 then overwrites with
	 * bits 18,17,5 if stepping is non-A0.  On C0 (X61) bit 18 is
	 * always set regardless of cold/warm boot.
	 */
	uint8_t stepping = northbridge_stepping();
	uint32_t rcomp_ctrl = mchbar_read32(RCOMP_CTRL);
	rcomp_ctrl &= 0xfffaffee;  /* Clear bits 18, 4, and 0 (stale GO bit) */
	rcomp_ctrl |= 0x20020;     /* Set bits 17, 5 */
	mchbar_write32(RCOMP_CTRL, rcomp_ctrl);
	if (stepping != 0x00) {
		rcomp_ctrl |= 0x60020; /* Non-A0: also set bit 18 */
		mchbar_write32(RCOMP_CTRL, rcomp_ctrl);
	}

	/*
	 * Step 2: RCOMP_STATUS - keep bits 15,11,7,3; set bits 12,8,4,0.
	 * Vendor uses 16-bit access (trace: "=> 2222 / <= 1111").
	 * Must NOT use 32-bit access: that would zero out the adjacent
	 * register at 0x0406-0x0407, corrupting RCOMP engine state.
	 */
	mchbar_clrsetbits16(RCOMP_STATUS, ~0x8888 & 0xffff, 0x1111);

	/*
	 * Step 3: RCOMP_CFG - clear bits [13:9], set 13,11,10,9 = 0x2E00.
	 * Vendor uses 16-bit access (trace: "=> 0100 / <= 2f00").
	 * Must NOT use 32-bit access: that would zero out the adjacent
	 * register at 0x040e-0x040f, preventing RCOMP from completing.
	 */
	mchbar_clrsetbits16(RCOMP_CFG, ~0xc1ff & 0xffff, 0x2e00);

	/* Step 4: RCOMP_CFG3 - set bit 18 */
	mchbar_setbits32(RCOMP_CFG3, 1 << 18);

	/* Step 5: RCOMP_CFG4 - clear even nibble bits, set pattern */
	mchbar_clrsetbits32(RCOMP_CFG4, ~0x99999999U, 0x11119999);

	/* Step 6: RCOMP_ODT0/1 - keep bits [7:6], set 0x36 in [5:0] */
	mchbar_clrsetbits8(RCOMP_ODT0, 0x3f, 0x36);
	mchbar_clrsetbits8(RCOMP_ODT1, 0x3f, 0x36);

	/*
	 * Step 7: Copy ROM compensation tables to MCHBAR RCOMP_TABLES.
	 * 9 groups, each occupying 64 bytes in MCHBAR (with gaps).
	 *
	 * BIOS layout per group in MCHBAR (64 bytes):
	 *   Offsets 0x00-0x0B: ROM entries 0-2 (12 bytes)
	 *   Offsets 0x0C-0x17: gap (skipped)
	 *   Offsets 0x18-0x23: ROM entries 3-5 (12 bytes)
	 *   Offsets 0x24-0x2F: gap (skipped)
	 *   Offsets 0x30-0x3F: ROM entries 6-9 (16 bytes)
	 *
	 * Must use mchbar_write32() (volatile MMIO accessor) - raw pointer
	 * writes through a non-volatile uint32_t * can be reordered or
	 * eliminated by the compiler, leaving the RCOMP engine with empty
	 * tables and preventing calibration from completing.
	 */
	for (g = 0; g < 9; g++) {
		uint16_t tbl_off = RCOMP_TABLES + g * 64;

		/* First 3 DWORDs (entries 0-2) at group offset 0x00 */
		for (i = 0; i < 3; i++)
			mchbar_write32(tbl_off + i * 4, rcomp_rom_table[g][i]);

		/* Next 3 DWORDs (entries 3-5) at group offset 0x18 */
		for (i = 0; i < 3; i++)
			mchbar_write32(tbl_off + 0x18 + i * 4, rcomp_rom_table[g][3 + i]);

		/* Last 4 DWORDs (entries 6-9 = strength) at group offset 0x30 */
		for (i = 0; i < 4; i++)
			mchbar_write32(tbl_off + 0x30 + i * 4, rcomp_rom_table[g][6 + i]);
	}

	/*
	 * Step 8: Program RCOMP_CFG2 clock-dependent value.
	 * BIOS: DAT_fed14414 = lookup[mem_clock] + lookup[fsb_clock]
	 */
	/*
	 * Clock-dependent RCOMP comp codes - from BIOS ROM at 0xFFFF5904/5909
	 * and verified against inteltool dump (RCOMP_CFG2 = 0x50 for FSB800+DDR2-667).
	 *
	 * Vendor BIOS uses its own sysinfo indices (0x1c, 0x1d) which differ
	 * from our fsb_clock / mem_clock.  Mapped to our enum values:
	 *   fsb_clock: FSB_CLOCK_533MHz=1, FSB_CLOCK_800MHz=2, FSB_CLOCK_667MHz=3
	 *   mem_clock: MEM_CLOCK_533MT=1, MEM_CLOCK_667MT=2
	 *
	 * ROM FSB table {0x00,0x01,0x05,0x00} @ vendor idx 0-3
	 * ROM MEM table {0x00,0x10,0x50,0x00} @ vendor idx 0-3
	 * Vendor FSB idx = fsb_clock - 2; MEM idx = mem_clock (direct).
	 */
	static const uint8_t rcomp_fsb_codes[] = { 0, 0x00, 0x00, 0x01 };  /* indexed by fsb_clock */
	static const uint8_t rcomp_mem_codes[] = { 0, 0x10, 0x50, 0x00 };  /* indexed by mem_clock */
	uint8_t rcomp_code = rcomp_fsb_codes[si->timings.fsb_clock] +
		rcomp_mem_codes[si->timings.mem_clock];
	mchbar_write8(RCOMP_CFG2, rcomp_code);

	/*
	 * Step 9: Re-arm RCOMP comparator with a latch write.
	 *
	 * Vendor (fff010fd): reads MCHBAR[0x0418] then writes the same
	 * value back - a latch write that re-arms the comparator before
	 * the GO trigger.  Do NOT clear bit 17 here: bit 17 is already 0
	 * at this point (only bit 18 was set in step 4), and incorrectly
	 * clearing it causes the teardown write to produce 0x00060000
	 * (bits 17+18) instead of the correct 0x00040000 (bit 18 only).
	 */
	mchbar_write32(RCOMP_CFG3, mchbar_read32(RCOMP_CFG3));

	/*
	 * Step 10: Start RCOMP - set bit 0 (GO).
	 * A1 stepping only: also set bit 1 (vendor: stepping == 0x01).
	 * On A0 and C0 (all others): bit 0 only.
	 *
	 * Vendor uses 8-bit access for the GO trigger (trace: "=> 20 / <= 21")
	 * to avoid disturbing upper bytes of RCOMP_CTRL which may contain
	 * hardware-managed state.
	 */
	if (stepping == 0x01)
		mchbar_setbits8(RCOMP_CTRL, 3);
	else
		mchbar_setbits8(RCOMP_CTRL, 1);

	printk(BIOS_DEBUG, "%s: started RCOMP_CTRL=0x%08x "
	       "CFG=0x%08x CFG2=0x%02x CFG3=0x%08x CFG4=0x%08x\n",
	       __func__,
	       mchbar_read32(RCOMP_CTRL),
	       mchbar_read32(RCOMP_CFG),
	       mchbar_read8(RCOMP_CFG2),
	       mchbar_read32(RCOMP_CFG3),
	       mchbar_read32(RCOMP_CFG4));
	/* Dump first RCOMP table entries to verify table data in binary.
	 * Vendor inteltool: 0x688=0x3cf3cf38, 0x6c8=0x34d34d30 */
	printk(BIOS_DEBUG, "%s: table[0x688]=0x%08x "
	       "table[0x6c8]=0x%08x table[0x6b0]=0x%08x\n",
	       __func__,
	       mchbar_read32(RCOMP_TABLES + 0x08),
	       mchbar_read32(RCOMP_TABLES + 0x48),
	       mchbar_read32(RCOMP_TABLES + 0x30));
}

/* ================================================================== */
/* DDR2 ODT Setup                                                     */
/* ================================================================== */

/*
 * CxODT timing lookup tables, extracted from BIOS ROM at 0xFFFF5AB8.
 * Indexed by CAS latency (3..6), each entry is {ODT_LOW_mask, ODT_HIGH_mask}.
 * These are OR'd with the existing register value (after masking).
 *
 * CxODT_LOW mask:  & 0x8f3f8f88
 * CxODT_HIGH mask: & 0x9f480000
 */
static const uint32_t odt_timing_table[4][2] = {
	/* CAS 3 */ { 0x20001010, 0x60918788 },  /* from ROM at index 0 */
	/* CAS 4 */ { 0x20002020, 0x60928788 },  /* from ROM at index 1 */
	/* CAS 5 */ { 0x20003030, 0x60938788 },  /* from ROM at index 2 */
	/* CAS 6 */ { 0x20004040, 0x60948788 },  /* from ROM at index 3 */
};

/*
 * odt_pre_setup() - Pre-JEDEC ODT register setup
 *
 * BIOS equivalent: FFFF4036 / FFF01412 (89 bytes, POST 0xFF24)
 * Sets CxDRA_HI bits and CxDRT1 bit 15 for 8-bank DIMMs.
 *
 * Vendor decompilation (FFF01412):
 *   if (present && banks_flag)     // banks_flag = SPD[17]>>3
 *       CxDRA_HI |= 0x09;
 *       CxDRT1   |= 0x8000;
 *
 * The condition gates on 8-bank SDRAM devices (SPD byte 17 == 8),
 * NOT on dual-rank.  banks_flag in the vendor per-DIMM struct is
 * byte 3 = SPD[0x13] >> 3, which is 1 for 8 banks and 0 for 4.
 */
static void odt_pre_setup(sysinfo_t *si)
{
	int ch;
	for (ch = 0; ch < 2; ch++) {
		int slot = ch * 2;
		/* Clear lower byte of CxDRA upper half */
		mchbar_clrbits16(CxDRA_HI(ch), 0x00ff);
		if (slot < 4 && si->dimms[slot].present
		    && si->dimms[slot].banks == 8) {
			mchbar_setbits16(CxDRA_HI(ch), 0x09);
			mchbar_setbits32(CxDRT1_MCHBAR(ch), 0x8000);
		}
	}
}

/*
 * misc_odt_settings() - Program CxODT_LOW/HIGH from ROM lookup table
 *
 * BIOS equivalent: FFFF4400 (119 bytes, POST 0xFF26)
 * These timing values depend on CAS latency.
 */
static void misc_odt_settings(sysinfo_t *si)
{
	int cas_idx = si->timings.CAS - 3;
	if (cas_idx < 0)
		cas_idx = 0;
	if (cas_idx > 3)
		cas_idx = 3;
	int ch;
	for (ch = 0; ch < 2; ch++) {
		mchbar_clrsetbits32(CxODT_LOW(ch),  ~0x8f3f8f88U, odt_timing_table[cas_idx][0]);
		mchbar_clrsetbits32(CxODT_HIGH(ch), ~0x9f480000U, odt_timing_table[cas_idx][1]);
	}
}

/*
 * ddr2_odt_setup() - Configure On-Die Termination control registers
 *
 * BIOS equivalent: FFFF4477 (252 bytes, POST 0xFF27)
 * Programs ODT misc, timing, and global write/arbitration registers.
 */
static void ddr2_odt_setup(sysinfo_t *si)
{
	int ch;

	/*
	 * Per-channel ODT control registers.
	 *
	 * Vendor ROM table at 0xFFFF5A88 is indexed by
	 * [fsb_clkcfg_code][CAS_index], NOT [our_fsb_enum][mem_clock].
	 * For FSB800 (clkcfg=2) and FSB667 (clkcfg=3), the table rows
	 * are identical: value = (CAS - 3) + 4 = CAS + 1.
	 * Verified: inteltool CxODT_MISC = 0x80000006 (bits[4:0]=6=CAS+1).
	 *
	 * ODT_TIMING: vendor uses CAS_index + 2 = (CAS-3) + 2 = CAS - 1.
	 * Verified: inteltool CxODT_TIMING = 0x00000004 (bits[3:0]=4=CAS-1).
	 */
	uint8_t odt_misc_val = si->timings.CAS + 1;

	for (ch = 0; ch < 2; ch++) {
		/* CxODT_MISC bit 31: vendor sets this first (32-bit OR) */
		mchbar_setbits32(CxODT_MISC(ch), 0x80000000);

		/* CxODT_MISC low byte: keep bits [7:5], set lower 5 from lookup.
		 * Vendor uses 8-bit access (mov byte) - NOT 32-bit. */
		mchbar_clrsetbits8(CxODT_MISC(ch), 0x1f, odt_misc_val);

		/* CxODT_TIMING: low nibble = CAS - 1 */
		mchbar_clrsetbits8(CxODT_TIMING(ch), 0x0f, si->timings.CAS - 1);

		/* CxODT_CTRL: bits [2:0] = 2 */
		mchbar_clrsetbits8(CxODT_CTRL(ch), 0x07, 0x02);
	}

	/*
	 * Global write/arbitration registers.
	 * Vendor (odt_setup_FF27, FFFF4525-FFFF454E): WRITE_CTRL bit 4
	 * is set only on A0/A1 steppings (CMP [0xF0000008], 1; JA skip).
	 * On C0+ steppings, bit 4 is NOT set.
	 */
	uint32_t wr_ctrl = mchbar_read32(WRITE_CTRL) & 0x113ff3ff;
	wr_ctrl |= 0x86000400;
	if (northbridge_stepping() <= 0x01)
		wr_ctrl |= 0x10;  /* A0/A1 only */
	mchbar_write32(WRITE_CTRL, wr_ctrl);

	mchbar_clrsetbits32(MMARB0, ~0xfff9ffffU, 0x210000);
	mchbar_clrsetbits32(MMARB1, ~0xfffffbffU, 0x300);
}

/* ================================================================== */
/* DDR2 Memory IO Init                                                */
/* ================================================================== */

/*
 * ddr2_memory_io_init() - Configure DDR2 memory I/O (POST 0xFF28)
 *
 * Programs IO_INIT_CFG, DRAM_TYPE_SELECT, and related IO registers.
 *
 * BIOS equivalent: FFFF3D71 (72 bytes, POST 0xFF28)
 */
static void ddr2_memory_io_init_phase1(sysinfo_t *si)
{
	/* IO_INIT_CFG: clear bits 21,19,17,16; set bit 20 */
	mchbar_clrsetbits32(IO_INIT_CFG, 0x002e0000, 0x100000);

	/* IO_INIT_CFG7: set RCOMP code 0x36 */
	mchbar_setbits8(IO_INIT_CFG7, 0x36);

	/* DRAM_TYPE_SELECT: set channel enable bits based on rank population */
	int ch;
	for (ch = 0; ch < 2; ch++) {
		if (si->dimms[ch * 2].present)
			mchbar_setbits32(DRAM_TYPE_SELECT, 0x80000000U >> ch);
	}
	mchbar_setbits32(DRAM_TYPE_SELECT, 0x4080);  /* DDR2 mode + misc */
}

/*
 * ddr2_memory_io_init_phase2() - Clock-dependent IO configuration (POST 0xFF29)
 *
 * BIOS equivalent: FFFF3DB9 (192 bytes, POST 0xFF29)
 */
static void ddr2_memory_io_init_phase2(sysinfo_t *si)
{
	int bytelane, ch;

	/*
	 * IO_INIT_CLK_DEP: clear bits [12:9].
	 * Vendor uses 16-bit access (AND 0xe1ff) to avoid disturbing
	 * upper 16 bits at 0x140e which contain IO config state.
	 */
	mchbar_clrbits16(IO_INIT_CLK_DEP, 0x1e00);

	/* IO_INIT_CFG2: clock-dependent value from vendor ROM at 0xFFFF58FC.
	 * Verified: inteltool 0x1414 bits[20:17] = 0x0a for DDR2-667. */
	static const uint8_t io_cfg2_table[] = { 0, 0x0d, 0x0a, 0x08 };
	uint32_t cfg2_val = (uint32_t)io_cfg2_table[si->timings.mem_clock] << 17;
	mchbar_clrsetbits32(IO_INIT_CFG2, 0x001f0000, cfg2_val);

	/* IO_INIT_CFG3/CFG4: clear bit 0 of each byte */
	mchbar_clrbits32(IO_INIT_CFG3, 0x01010101);
	mchbar_clrbits32(IO_INIT_CFG4, 0x01010101);

	/* IO_INIT_CFG5: clock-dependent lower nibble from vendor ROM at 0xFFFF58E8.
	 * Verified: inteltool 0x142c low nibble = 0x0a for DDR2-667. */
	static const uint8_t io_cfg5_table[] = { 0, 0x0b, 0x0a, 0x00 };
	mchbar_clrsetbits8(IO_INIT_CFG5, 0x0f, io_cfg5_table[si->timings.mem_clock]);

	/* IO_INIT_CFG6: clear bits [21:20] */
	mchbar_clrbits32(IO_INIT_CFG6, 0x300000);

	/* Per-channel PI (Phase Interpolator) settings from vendor ROM at 0xFFFF58EC.
	 * Controls DQ byte lane timing - critical for data integrity.
	 * Verified: inteltool 0x1490-0x14ac = 0x00001111 for DDR2-667. */
	static const uint32_t pi_table[] = { 0, 0x00002121, 0x00001111, 0x00000000 };
	uint32_t pi_val = pi_table[si->timings.mem_clock];
	/* Vendor interleaves ch0/ch1 per byte lane. */
	for (bytelane = 0; bytelane < NUM_BYTELANES; bytelane++) {
		for (ch = 0; ch < NUM_CHANNELS; ch++)
			mchbar_write32(CxTRAIN_PI(ch) + bytelane * 4, pi_val);
	}

	/* Per-channel training config: clear bit 0 if channel populated */
	for (ch = 0; ch < 2; ch++) {
		if (si->dimms[ch * 2].present)
			mchbar_clrbits8(CxTRAIN_CFG(ch), 0x01);
	}
}

/*
 * program_rw_pointer_and_ddr_type() - Configure per-channel RW pointer & TYPE select (POST 0xFF30)
 *
 * BIOS equivalent: FFFF3E79 (46 bytes, POST 0xFF30)
 */
static void program_rw_pointer_and_ddr_type(sysinfo_t *si)
{
	int ch;
	for (ch = 0; ch < 2; ch++) {
		if (si->dimms[ch * 2].present)
			mchbar_setbits32(RW_PTR_CTRL(ch), 0x300);
	}
	mchbar_setbits8(DRAM_TYPE_SELECT, 0x40);
}

/* ================================================================== */
/* DRAM Clock Enable                                                  */
/* ================================================================== */

static void enable_dram_clocks(const sysinfo_t *si)
{
	int ch;
	for (ch = 0; ch < 2; ch++) {
		int slot = ch * 2;
		if (si->dimms[slot].present)
			mchbar_setbits32(CxDCLKDIS_MCHBAR(ch), 0x3);  /* Enable both clocks - 32-bit per vendor */
	}
}

/* ================================================================== */
/* JEDEC DDR2 Initialization Sequence                                 */
/* ================================================================== */

/*
 * jedec_command() - Issue a single JEDEC command via DCC register
 *
 * The GM965 DCC register at MCHBAR+0x200 controls DRAM commands.
 * Writing a command code to bits [18:16] initiates the command.
 * The rank address is accessed via a read to the rank's base address.
 *
 * coreboot equivalent: jedec_command() in raminit.c
 */
static void jedec_command(uintptr_t rankaddr, uint32_t cmd, uint32_t val)
{
	mchbar_clrsetbits32(DCC_MCHBAR, DCC_SET_EREG_MASK, cmd);
	read32((void *)(uintptr_t)(rankaddr | val));
}

/*
 * get_rank_addr() - Get the base address for a rank
 *
 * Returns the physical address where rank (ch, rank) starts, derived
 * from the DRB boundary registers written by prejedec_memory_map().
 * Rank N's start address = rank (N-1)'s upper boundary x 32 MB.
 *
 * Vendor approach (FFFF4573, FFFF5A98 ROM table):
 *   The vendor stores 8 pointers to individual 16-bit DRB entries
 *   in a ROM table at FFFF5A98.  The JEDEC loop reads each pointer,
 *   dereferences it to get the DRB value, then does `<< 25` to
 *   convert 32 MB units to a byte address.  The loop starts with
 *   rank_addr = 0 and computes the next rank's address at the end
 *   of each iteration.
 *
 * This function produces the same result via computed register
 * offsets: read the previous rank's DRB, extract the boundary
 * from the correct 16-bit half, and shift left by 25.
 */
static uintptr_t get_rank_addr(int ch, int rank)
{
	if (ch == 0 && rank == 0)
		return 0;

	/* Start address = upper bound of the previous rank. */
	int prev_ch = ch, prev_rank = rank - 1;
	if (prev_rank < 0) {
		prev_ch = ch - 1;
		prev_rank = 3; /* highest rank index per channel */
	}
	uint32_t reg = mchbar_read32(CxDRBy_MCHBAR(prev_ch, prev_rank));
	/* Boundary in 32 MB units: even ranks in bits [8:2], odd in bits [24:18]. */
	uint32_t shift = (prev_rank % 2) * 16;
	return ((reg >> shift) & 0x1fc) << 25;
}

/*
 * jedec_init_ddr2() - Run DDR2 JEDEC initialization sequence
 *
 * Per-rank command sequence (vendor FFF0194F, 635 bytes):
 *   NOP -> bit15 -> ABP -> EMRS2 -> EMRS3 -> EMRS1(ODT) ->
 *   MRS(DLL reset) -> ABP -> CBR -> CBR -> MRS(normal) ->
 *   EMRS1(OCD default) -> EMRS1(OCD exit) -> clear bit15
 *
 * BIOS equivalent: FFF0194F (RAMINIT copy, POST 0xFF33, cold boot only)
 */
static void jedec_init_ddr2(sysinfo_t *si)
{
	timings_t *t = &si->timings;

	/*
	 * DDR2 MRS/EMRS bit encodings (address lines, shifted by 3 for bus width).
	 * These match the coreboot jedec_init_ddr2() implementation.
	 */
	int WR = ((t->tWR - 1) & 7) << 12;
	int DLLreset = 1 << 11;
	int CAS = (t->CAS & 7) << 7;
	int BTinterleaved = 1 << 6;
	int BL8 = 3 << 3;              /* Burst Length 8 */
	int OCDdefault = 7 << 10;      /* OCD Calibration Default */
	int ODT_150OHMS = (1 << 9);    /* EMRS1 ODT = 150 ohm */

	int ch, r;

	/*
	 * Pre-JEDEC setup (FSBPMC3, SBTEST, POST_JEDEC_TIM0/1)
	 * and write pointer enables (RW_PTR_CTRL bits [9:8]) are already done
	 * by the sequencer at POST 21 and POST 30 (vc1_isoch_timings).
	 * The vendor FFF0194F does NOT repeat them here.
	 */

	/* JEDEC init sequence for each populated rank */
	for (ch = 0; ch < 2; ch++) {
		int slot = ch * 2;
		if (!(slot < 4 && si->dimms[slot].present))
			continue;

		int max_rank = si->dimms[slot].dual_rank ? 2 : 1;
		for (r = 0; r < max_rank; r++) {
			uintptr_t raddr = get_rank_addr(ch, r);

			printk(BIOS_DEBUG, "JEDEC init: ch%d rank%d addr=0x%08lx\n",
			       ch, r, (unsigned long)raddr);

			/*
			 * Vendor (FFF019CA-FFF019E8): issue NOP first,
			 * then set bit 15 (JEDEC init active).  Ordering
			 * matters - NOP must precede the JEDEC-active flag.
			 */
			mchbar_clrsetbits32(DCC_MCHBAR, DCC_CMD_MASK,
				DCC_CMD_NOP);
			mchbar_setbits32(DCC_MCHBAR, 0x8000);

			/* 1. All Banks Precharge */
			jedec_command(raddr, DCC_CMD_ABP, 0);

			/* 2. EMRS2 (MR2) = 0 */
			jedec_command(raddr, DCC_SET_EREGx(2), 0);

			/* 3. EMRS3 (MR3) = 0 */
			jedec_command(raddr, DCC_SET_EREGx(3), 0);

			/* 4. EMRS1 (MR1): enable ODT */
			jedec_command(raddr, DCC_SET_EREG, ODT_150OHMS);

			/* 5. MRS (MR0): DLL reset + CAS + BL */
			jedec_command(raddr, DCC_SET_MREG,
				WR | DLLreset | CAS | BTinterleaved | BL8);

			/* 6. All Banks Precharge */
			jedec_command(raddr, DCC_CMD_ABP, 0);

			/* 7. CBR Auto-Refresh (x2) */
			jedec_command(raddr, DCC_CMD_CBR, 0);
			udelay(1);
			read32((void *)(uintptr_t)raddr);  /* second CBR via read */

			/* 8. MRS (MR0): normal (no DLL reset) */
			jedec_command(raddr, DCC_SET_MREG,
				WR | CAS | BTinterleaved | BL8);

			/*
			 * 9-10. EMRS1: OCD Calibration Default, then Exit.
			 *
			 * Vendor (FFF01B79-FFF01B99): sets DCC to EMRS1
			 * once, then does two memory reads - OCD cal
			 * (raddr | 0x1E00) then OCD exit (raddr | 0x200).
			 * No DCC re-write between them since the command
			 * type is the same.
			 */
			mchbar_clrsetbits32(DCC_MCHBAR, DCC_SET_EREG_MASK,
				DCC_SET_EREG);
			read32((void *)(uintptr_t)(raddr
				| OCDdefault | ODT_150OHMS));
			read32((void *)(uintptr_t)(raddr | ODT_150OHMS));

			/* Clear DCC bit 15 - JEDEC init complete for this rank */
			mchbar_clrbits32(DCC_MCHBAR, 0x8000);
		}
	}
}

/*
 * post_jedec_sequence() - Transition to normal operation
 *
 * BIOS equivalent: FFFF3909 (37 bytes, POST 0xFF34)
 *
 * Clears DRAM_TYPE_SELECT bit 9, sets DCC interleaved mode
 * for dual channel, and sets DCC bit 10 (normal operation announce).
 */
static void post_jedec_sequence(sysinfo_t *si)
{
	/*
	 * Clear DRAM_TYPE_SELECT bit 9 - use 16-bit access to avoid
	 * disturbing upper 16 bits (bits 31/30 = channel enables set
	 * at POST 28).  Vendor: 16-bit AND 0xFDFF.
	 */
	mchbar_clrbits16(DRAM_TYPE_SELECT, 0x200);

	/*
	 * DCC: clear interleave bit, then set if dual-channel.
	 * BIOS: sets DCC_INTERLEAVED for any nonzero channel_mode
	 * (i.e., both DUAL_ASYNC and DUAL_INTERLEAVED).
	 */
	mchbar_clrbits32(DCC_MCHBAR, DCC_INTERLEAVED);
	if (si->timings.channel_mode != CHANNEL_MODE_SINGLE)
		mchbar_setbits32(DCC_MCHBAR, DCC_INTERLEAVED);

	/* DCC: set bit 10 - announce normal operation */
	mchbar_setbits32(DCC_MCHBAR, 0x400);
}

/* ================================================================== */
/* Final Timing Adjust (POST 0xFF36)                                  */
/* ================================================================== */

/*
 * final_timing_adjust() - Post-JEDEC clock crossing and timing fixups
 *
 * BIOS equivalent: FFFF47A4 (125 bytes, POST 0xFF36)
 * Sets per-channel clock crossing constants, clears pre-JEDEC bits,
 * and programs final SBTEST / POST_JEDEC_TIM values.
 */
static void final_timing_adjust(sysinfo_t *si)
{
	/* Clear DCC bits [10:9] for dual-channel */
	if (si->timings.channel_mode == CHANNEL_MODE_DUAL_INTERLEAVED)
		mchbar_clrbits32(DCC_MCHBAR, 0x600);

	/* Per-channel clock crossing constants (hardcoded from BIOS) */
	int ch;
	for (ch = 0; ch < 2; ch++) {
		mchbar_write32(CxAIT_LO(ch), 0x000006c4);
		mchbar_write32(CxAIT_HI(ch), 0x871a066d);
	}

	/* Clear CTRL0 bit 1 (pre-JEDEC mode) */
	mchbar_clrbits32(FSBPMC3, 1 << 1);

	/* Final SBTEST, POST_JEDEC_TIM0/1 values from BIOS */
	mchbar_clrsetbits32(SBTEST,  0x00080006U, 0x8000);
	mchbar_clrsetbits32(POST_JEDEC_TIM0, 0x03404900U, 0x04bdb600);
	mchbar_clrsetbits32(POST_JEDEC_TIM1, 0x03c04900U, 0x003db600);
}

/* ================================================================== */
/* DRAM Optimizations (POST 0xFF37)                                   */
/* ================================================================== */

/*
 * dram_optimizations() - Set CxDRC1 SSDS field based on rank topology
 *
 * BIOS equivalent: FFFF4821 (106 bytes, POST 0xFF37)
 * The SSDS (Self-refresh Single DIMM Suspend) field in the upper byte
 * of CxDRC1 depends on how many ranks are populated per channel.
 */
static void dram_optimizations(sysinfo_t *si)
{
	/*
	 * SSDS lookup: indexed by rank topology per channel.
	 *   0 = no DIMM present
	 *   1 = at least one single-rank DIMM in channel
	 *   2 = all DIMMs dual-rank (no single-rank DIMMs)
	 * Values from BIOS ROM at 0xFFFF57C9 (stride 2: bytes 0,2,4).
	 * Verified: inteltool CxDRC1[31:24] = 0xb1 for dual-rank DIMMs.
	 */
	static const uint8_t ssds_table[] = { 0x00, 0x91, 0xb1 };

	/*
	 * Stepping-dependent SSDS override.
	 *
	 * Vendor (FFF01C0D): On non-A0 steppings < 4 (including C0),
	 * the rank topology index is forced to 2 for all populated
	 * channels, regardless of whether the DIMM is actually
	 * dual-rank.  This gives SSDS = 0xb1 for both single-rank
	 * and dual-rank DIMMs on C0 stepping.
	 *
	 * Without this, single-rank DIMMs get SSDS = 0x91 while the
	 * vendor always uses 0xb1 on C0.
	 */
	uint8_t stepping = northbridge_stepping();
	int force_dual_ssds = (stepping != 0x00 && stepping < 0x04);

	int ch;
	for (ch = 0; ch < 2; ch++) {
		int slot = ch * 2;
		int rank_count = 0;
		if (slot < 4 && si->dimms[slot].present) {
			rank_count = si->dimms[slot].dual_rank ? 2 : 1;
			if (force_dual_ssds)
				rank_count = 2;
		}
		mchbar_clrsetbits32(CxDRC1_MCHBAR(ch), 0xff000000,
			(uint32_t)ssds_table[rank_count] << 24);
	}
}

/* ================================================================== */
/* EPD Address Decode and Channel Enable (POST 0xFF41)             */
/* ================================================================== */

/*
 * EPD timing lookup tables for FFF02A7A.
 *
 * These are extracted from the RAMINIT copy's stack-local arrays.
 * Indexed by mem_clock (1=800MT, 2=667MT, 3=533MT).
 */

/* tWTR equivalent for EPD (same as drt0_twtr_lut) */
static const uint8_t epd_twtr[] = {
	[0]               = 2,
	[MEM_CLOCK_533MT] = 2,
	[MEM_CLOCK_667MT] = 3,
};

/* tRFC multiplier for EPD+0x19 bits [15:11] */
static const uint8_t epd_trfc_mult[] = {
	[0]               = 9,
	[MEM_CLOCK_533MT] = 12,
	[MEM_CLOCK_667MT] = 15,
};

/*
 * Slave-DLL-enable to read delay for EPD+0x24 (Tsdllen2nd in EGLK).
 * Note: EGLK indexes this table starting from DDR2-400 (highest index =
 * lowest frequency), opposite to the gm965 mem_clock enum ordering.
 */
static const uint8_t epd_rtp_pchg[] = {
	[0]               = 4,
	[MEM_CLOCK_533MT] = 6,
	[MEM_CLOCK_667MT] = 7,
};

/* tRTP alternate for EPD+0x22 */
static const uint8_t epd_trtp_alt[] = {
	[0]               = 2,
	[MEM_CLOCK_533MT] = 3,
	[MEM_CLOCK_667MT] = 4,
};

/*
 * epd_dra_encode() - Compute EPD DRA encoding for a DIMM
 *
 * Returns a 16-bit value: low byte = rank 0 DRA, high byte = rank 1
 * DRA (0 if single-rank, same as rank 0 if dual-rank).
 *
 * The produced encoding is identical to the CxDRA format used on x4x
 * (Eaglelake) and other desktop 9xx/x3x chipsets: bit 7 set means the
 * device uses a "large" geometry (>= 4 banks x >= 1KB page), and bits
 * [2:0] encode the row/col/width combination.
 *
 * BIOS equivalent: FFF02A7A DRA loop (fff02b5c-fff02bea)
 *
 * The index is computed by FFF022DF (timing calculation) at
 * fff02450-fff0245e and stored in per-DIMM sysinfo field[4]:
 *   MOV AL, [ESI+3]     ; banks_flag (SPD[17]>>3: 0 or 1)
 *   ADD AL, [ESI+2]     ; + x16_flag (0 or 1)
 *   ADD AL, [EDX+6]     ; + cols
 *   ADD AL, [EDX+5]     ; + rows
 *   SUB AL, 0x16        ; - 22
 *
 * This equals the standard chip capacity index + 1:
 *   standard = rows + cols + log2(width) + log2(banks) - 28
 *   vendor   = rows + cols + x16_flag + banks_flag - 22
 *            = standard + 1
 *
 * Switch on index:
 *   index=0 -> skip      index=1 -> base=0
 *   index=2 -> base=2+(banks==8?2:0)
 *   index=3 -> base=6    index=4 -> base=8
 *   result = base + x16;  if result > 3 -> result |= 0x80
 */
static uint16_t epd_dra_encode(const dimminfo_t *d)
{
	/*
	 * Vendor formula: rows + cols + x16_flag + banks_flag - 22
	 * where x16_flag = (chip_width==16)?1:0, banks_flag = (banks==8)?1:0.
	 *
	 * Verified against disasm fff02450 and inteltool for two DIMMs:
	 *   DIMM0 (13+10+1+1-22=3) -> base=6, +x16 -> enc=0x87 (ok)
	 *   DIMM2 (14+10+0+1-22=3) -> base=6, +0   -> enc=0x86 (ok)
	 */
	int idx = d->rows + d->cols
		+ (d->x16 ? 1 : 0)
		+ ((d->banks == 8) ? 1 : 0)
		- 22;

	if (idx < 0)
		idx = 0;
	if (idx > 4)
		idx = 4;

	int base;
	switch (idx) {
	case 0:
		return 0;  /* 256Mb or less - skip */
	case 1:
		base = 0;
		break;
	case 2:
		base = (d->banks == 8) ? 4 : 2;
		break;
	case 3:
		base = 6;
		break;
	case 4:
		base = 8;
		break;
	default:
		return 0;
	}

	uint8_t enc = base + (d->x16 ? 1 : 0);
	if (enc > 3)
		enc |= 0x80;

	/* Low byte = rank 0, high byte = rank 1 (0 if single-rank) */
	return (uint16_t)enc | (d->dual_rank ? ((uint16_t)enc << 8) : 0);
}

/*
 * Compute the EPD_A0 upper 16-bit field from the rank-population bitmap.
 * Replicates the vendor algorithm at FFF03088: walk each set bit index i;
 * if first set bit record dl=i, else dl=i+1; result = (1 << (dl+1)) << 16.
 */
static uint32_t rank_bitmap_to_upper16(uint8_t rank_bitmap)
{
	int dl = 0, first = 1;

	for (int i = 0; i < 5; i++) {
		if (rank_bitmap & (1 << i)) {
			if (first || dl == 0) {
				dl = i;
				first = 0;
			} else {
				dl = i + 1;
			}
		}
	}
	return (1U << (dl + 1)) << 16;
}

/*
 * program_epd() - Program EPD address decode and timing
 *
 * Programs the secondary address decode registers at MCHBAR 0x0A00
 * that the memory controller uses for channel routing, scheduling,
 * and the graphics engine's memory access path.
 *
 * Without this, the controller does not properly route transactions
 * to channel 1, causing only channel 0 to function.
 *
 * BIOS equivalent: FFF02A7A (1603 bytes, called between POST 0xFF41-0xFF42)
 *
 * The RAMINIT copy at FFF0xxxx (55 functions) is the BIOS's actual
 * execution path.  The RAMINIT3 copy at FFFF3xxx (29 functions) that
 * the rest of this file was based on omits this function entirely.
 */
static void program_epd(sysinfo_t *si)
{
	timings_t *t = &si->timings;
	int slot;
	unsigned int mc = t->mem_clock;
	unsigned int cas = t->CAS;
	uint32_t reg;

	/*
	 * Step 1: Copy rank boundary values from per-channel DRB regs
	 * to EPD.  Values are halved (>> 1) with bit 15 of each
	 * 16-bit half cleared.
	 *
	 * BIOS: two iterations copying CxDRBy(ch,0) and CxDRBy(ch,2)
	 * to EPD+0x00/0x04 (ch0) and EPD+0x34/0x38 (ch1).
	 */
	static const uint32_t epd_c0drb[2] = { EPD_C0DRB01, EPD_C0DRB23 };
	static const uint32_t epd_c1drb[2] = { EPD_C1DRB01, EPD_C1DRB23 };
	for (int r = 0; r < 2; r++) {
		uint32_t drb0 = mchbar_read32(CxDRBy_MCHBAR(0, r * 2));
		uint32_t drb1 = mchbar_read32(CxDRBy_MCHBAR(1, r * 2));
		mchbar_write32(epd_c0drb[r], (drb0 >> 1) & 0x7fff7fff);
		mchbar_write32(epd_c1drb[r], (drb1 >> 1) & 0x7fff7fff);
	}

	/*
	 * Step 2: Write DRA encoding for each DIMM slot.
	 * Ch0 DIMMs -> EPD_C0DRA01/C0DRA23 (16-bit per slot)
	 * Ch1 DIMMs -> EPD_C1DRA01/C1DRA23 (16-bit per slot)
	 *
	 * BIOS: loops 4 DIMMs, writes 16-bit values.
	 */
	static const uint32_t epd_dra[4] = {
		EPD_C0DRA01, EPD_C0DRA23,
		EPD_C1DRA01, EPD_C1DRA23,
	};
	for (slot = 0; slot < 4; slot++) {
		if (!si->dimms[slot].present)
			continue;
		mchbar_write16(epd_dra[slot], epd_dra_encode(&si->dimms[slot]));
	}

	/*
	 * Step 3: Program timing fields.
	 *
	 * EPD+0x1C is programmed below (32-bit RMW to match vendor).
	 */

	/*
	 * EPD+0x10: clear bit 2, set bit 3.
	 *
	 * Vendor (FFF02BFB): 32-bit RMW at 0xA10.  Must use 32-bit
	 * to match vendor bus transaction width.
	 */
	mchbar_clrsetbits32(EPD_10, 0x04, 0x08);

	/*
	 * EPD+0x11: low 5 bits = CAS + 8.
	 * Vendor: 32-bit unaligned RMW at 0xA11.
	 */
	mchbar_clrsetbits32(EPD_11, 0x1f, (cas + 8) & 0x1f);

	/*
	 * EPD+0x13: (old & 1) | 4  - set bit 2, keep bit 0
	 * Vendor: 8-bit access.
	 */
	mchbar_clrsetbits8(EPD_13, 0xfe, 0x04);

	/*
	 * EPD+0x14: low nibble = CAS
	 * Vendor: 8-bit access.
	 */
	mchbar_clrsetbits8(EPD_14, 0x0f, cas & 0x0f);

	/*
	 * EPD+0x15: bits [3:0] and bits [16:13] = CAS - 2
	 * BIOS: (old & 0xFFFE1FF0) | (CAS-2)*0x2000 | (CAS-2)
	 */
	mchbar_clrsetbits32(EPD_15, ~0xfffe1ff0U,
		(uint32_t)(cas - 2) | ((uint32_t)(cas - 2) << 13));

	/*
	 * EPD+0x19: three sub-fields packed in 16 bits.
	 *
	 *   bits [5:2]  = max(tWTR_tbank2[mc], 2) * 4 + 8
	 *   bits [10:6] = tRCD + CAS + 3
	 *   bits [15:11]= tRFC_mult[mc]
	 *
	 * BIOS (fff02c97): LEA ECX,[ECX+EAX+3] where ECX=tRCD, EAX=CAS.
	 *
	 * Vendor uses three separate 32-bit unaligned RMW at 0x0A19.
	 * Each reads DWORD, modifies only the lower 16 bits, writes
	 * DWORD back.  Must match vendor access width.
	 */
	{
		unsigned int twtr = epd_twtr[mc];
		if (twtr < 2)
			twtr = 2;
		unsigned int f1 = twtr * 4 + 8;
		unsigned int f2 = (t->tRCD + cas + 3);
		unsigned int f3 = epd_trfc_mult[mc];

		/* Step 1: bits [5:2] - vendor AND 0xFFFFFFC3 */
		mchbar_clrsetbits32(EPD_19, 0x3c, f1 & 0x3c);
		/* Step 2: bits [10:6] - vendor AND DX,0xF83F (16-bit AND) */
		mchbar_clrsetbits32(EPD_19, 0x07c0, (f2 & 0x1f) << 6);
		/* Step 3: bits [15:11] - vendor AND CH,0x07 */
		mchbar_clrsetbits32(EPD_19, 0xf800, (f3 & 0x1f) << 11);
	}

	/*
	 * EPD+0x1B: two nibbles.
	 *   low nibble  = CAS + 4
	 *   high nibble = tWTR + 3 + CAS
	 *
	 * Vendor: two separate 32-bit unaligned RMW at 0x0A1B.
	 * Step 1: AND 0xFFFFFFF0, OR (CAS+4)
	 * Step 2: AND DL,0x0F (byte-width AND), OR (btb_wtr << 4)
	 */
	{
		uint32_t lo = (cas + 4) & 0x0f;
		uint32_t hi = (epd_twtr[mc] + 3 + cas) & 0x0f;
		mchbar_clrsetbits32(EPD_1B, 0x0f, lo);
		mchbar_clrsetbits32(EPD_1B, 0xf0, hi << 4);
	}

	/*
	 * EPD+0x1C (EP channel timing): two 32-bit RMW operations.
	 *   bits [8:0]   = tRFC
	 *   bits [16:13] = tRRD (raw clocks)
	 */
	{
		mchbar_clrsetbits32(EPD_1C, 0x1ff, t->tRFC & 0x1ff);
		mchbar_clrsetbits32(EPD_1C, 0x1e000,
			(uint32_t)t->tRRD << 13);
	}

	/*
	 * EPD+0x20: multiple fields.
	 *   bits [3:0]  = tWTR + 3 + CAS (BtB_WtR equivalent)
	 *   bits [15:12]= tRP (raw cycle count)
	 *   bit  10     = 1 (constant)
	 * BIOS (fff02d4f): MOVZX EDI,[EBP-6]; SHL EDI,0xc - writes
	 * the restored tRP value directly (param[0x39]+2 = tRP).
	 * Previously added a spurious +2.
	 */
	{
		unsigned int btb_wtr = (epd_twtr[mc] + 3 + cas) & 0x0f;
		reg = mchbar_read32(EPD_20);
		reg = (reg & 0xfffffff0) | btb_wtr;
		reg = (reg & 0xffff04ff) | 0x400 | ((uint32_t)t->tRP << 12);
		mchbar_write32(EPD_20, reg);
	}

	/*
	 * EPD+0x22: bits [8:0] = tRTP_alt[mc] + tRFC
	 * Vendor: 32-bit unaligned RMW at 0x0A22.
	 */
	mchbar_clrsetbits32(EPD_22, 0x1ff,
		(epd_trtp_alt[mc] + t->tRFC) & 0x1ff);

	/*
	 * EPD+0x24: various fields including tRTP_precharge.
	 * BIOS: bits [14:12] = 0xe20, plus bStack_b shifted << 20 in
	 * the 32-bit view from 0x24.  The exact layout is:
	 *   (old & 0xFF8FCFE7) | 0xE20 | (rtp_pchg << 20)
	 */
	{
		reg = mchbar_read32(EPD_24);
		reg = (reg & 0xff8fcfe7) | 0xe20
		    | ((uint32_t)epd_rtp_pchg[mc] << 20);
		mchbar_write32(EPD_24, reg);
	}

	/*
	 * EPD+0x28: rank mode / scheduling.
	 *
	 * BIOS: clear bit 0 first, then:
	 *   iVar11 = 3 (no ch0 DIMM), or (ch0_single_rank?1:0)+1
	 *   (old & 0xFB07FD90) | 0x60190 | (iVar11 << 20) | 0x3000000
	 *
	 * Also: (old & 0xFFFF04FF) | rank-dependent bits for dual-rank
	 */
	{
		int rank_mode;
		int ch0_pop = si->dimms[0].present || si->dimms[1].present;
		if (!ch0_pop) {
			rank_mode = 3;
		} else {
			/*
			 * BIOS (fff02e02-fff02e12): checks first DIMM's
			 * dual_rank flag directly:
			 *   CMP [ESI+0x01], 0x01; SETZ CL; INC ECX
			 *   dual_rank -> rank_mode=2, single -> rank_mode=1
			 */
			int has_dual = 0;
			for (slot = 0; slot < 2; slot++) {
				if (si->dimms[slot].present
				    && si->dimms[slot].dual_rank)
					has_dual = 1;
			}
			rank_mode = has_dual ? 2 : 1;
		}

		mchbar_clrbits32(EPD_28, 1);
		reg = mchbar_read32(EPD_28);
		reg = (reg & 0xfb07fd90) | 0x60190
		    | ((uint32_t)rank_mode << 20)
		    | 0x3000000;
		mchbar_write32(EPD_28, reg);
	}

	/*
	 * EPD+0x2C: two 32-bit RMW operations.
	 * Vendor (fff02e43): AND 0xFFFFFFFA, OR 2 -> then AND CL,0x17 OR 0x10.
	 * Net result byte 0 = 0x12, but vendor bus transactions are 32-bit.
	 */
	mchbar_clrsetbits32(EPD_2C, 0x05, 0x02);
	mchbar_clrsetbits32(EPD_2C, 0xe8, 0x10);

	/*
	 * EPD+0x2D: set bits [4:0], clear bits [6:5].
	 * Vendor (fff02e67): 32-bit unaligned RMW at 0x0A2D.
	 */
	mchbar_clrsetbits32(EPD_2D, 0x60, 0x1f);

	/*
	 * EPD+0x30: mode control register.
	 *
	 * BIOS (fff02e79-fff02ee9) builds this in 5 steps, clearing
	 * fields before setting:
	 *   1. Lower 16: AND 0xc820, OR 0x0820 -> set bits 11,5
	 *   2. AND 0xfffd3fff, OR 0x10000 -> set bit 16
	 *   3. AND 0xffdbffff, OR 0x180000 -> set bits 19,20
	 *   4. AND 0xfd3fffff, OR 0x1000000 -> set bit 24
	 *   5. AND 0xdfffffff, OR 0x40000000 -> set bit 30
	 *
	 * Previous code only OR'd without clearing, leaving stale bits.
	 */
	{
		/*
		 * Vendor (FFF02E79) does 5 separate read-modify-write
		 * cycles.  The masks never touch bits [28:27] - those
		 * are preserved from the initial value.  Bits [28:27]
		 * are set by a post-raminit BIOS module (not raminit).
		 */
		uint32_t reg30 = mchbar_read32(EPD_30);
		/* Step 1: clear lower 16 except bits 15,14,11,5; set 11,5 */
		reg30 = (reg30 & 0xffffc820) | 0x820;
		/* Step 2: set bit 16, clear bits 17,15,14,13 */
		reg30 = (reg30 & 0xfffd1fff) | 0x10000;
		/* Step 3: set bits 19,20; clear bits 21,18 */
		reg30 = (reg30 & 0xffd3ffff) | 0x180000;
		/* Step 4: set bit 24; clear bits 25,23,22 */
		reg30 = (reg30 & 0xfd3fffff) | 0x1000000;
		/* Step 5: set bit 30; clear bit 29 */
		reg30 = (reg30 & 0xdfffffff) | 0x40000000;
		mchbar_write32(EPD_30, reg30);
	}

	/*
	 * EPD+0x99: CAS-dependent scheduling.
	 *   bits [10:9] = CAS - 3 (vendor CAS encoding)
	 * BIOS: multiple field writes in 0x99/0x9C region.
	 */
	{
		unsigned int cas_enc = cas - 3;
		mchbar_clrsetbits32(EPD_99, 0x0600, cas_enc << 9);
		mchbar_clrbits32(EPD_99, 0x000f0000);

		/*
		 * EPD+0x9C: additional CAS and rank-dependent bits.
		 * BIOS programs this region with complex multi-step RMW.
		 * The key patterns are:
		 *   bits [15:0]: 0x55 | rank-dependent
		 *   bits [16]: CAS-dependent
		 *   bits [19:17]: CAS-3
		 *   bits [22:20]: CAS-3
		 *   bits [24:23]: CAS
		 *   bit  [25]: 1
		 *   bits [31:26]: 0x55 pattern
		 */
		int ch0_pop = si->dimms[0].present || si->dimms[1].present;
		int rank_adj = 0;
		if (ch0_pop) {
			/*
			 * BIOS (fff02f14-fff02f22):
			 *   DEC DL; NEG DL; SBB EDX,EDX; ADD EDX,0xa
			 *   dual_rank -> rank_adj=10, single -> rank_adj=9
			 */
			int has_dual = 0;
			for (slot = 0; slot < 2; slot++) {
				if (si->dimms[slot].present
				    && si->dimms[slot].dual_rank)
					has_dual = 1;
			}
			rank_adj = has_dual ? 10 : 9;
		}

		reg = mchbar_read32(EPD_9C);
		reg &= 0xfff88855;
		/*
		 * BIOS (fff02f38-fff02f85) builds lower 16 bits:
		 *   byte[0xa9c] = 0x55 (AND 0xf5|0x05, AND 0x5f|0x50)
		 *   byte[0xa9d] = 0x88 (AND 0xf8|0x08, AND 0x8f|0x80)
		 * Previously had 0x88 which put the 0x88 into the wrong
		 * byte, producing 0x00dd instead of 0x8855.
		 */
		reg |= 0x8855 | ((uint32_t)cas_enc << 16)
		     | ((uint32_t)cas_enc << 19)
		     | ((uint32_t)cas << 22)
		     | (1 << 25);
		mchbar_write32(EPD_9C, reg);

		/* Clear bit 25 after setting (latch) */
		mchbar_clrbits32(EPD_9C, 1 << 25);

		/*
		 * EPD_9C bit 31: timing commit flag.
		 *
		 * Vendor BIOS (FFF02FE9-FFF02FF8) sets bit 31 as the
		 * FINAL step after all CAS/tRAS timing fields are
		 * programmed.  The sequence is:
		 *   AND  EAX, 0x0FFFFFFF   ; clear bits [31:28]
		 *   OR   EAX, 0x80000000   ; set bit 31
		 *   MOV  [0xfed14a9c], EAX
		 *
		 * Inteltool confirms: vendor=0x81528855 (bit 31 SET),
		 * coreboot=0x015200dd (bit 31 CLEAR).
		 */
		mchbar_clrsetbits32(EPD_9C, 0xf0000000U, 0x80000000U);

		/* Apply rank adjustment to bits [23:20] of EPD_99 */
		if (rank_adj) {
			/*
			 * BIOS (fff02f2b): SHL EDX, 0x14 (<<20, not <<16)
			 * BIOS (fff02f2e): AND EBX, 0xff0fffff (clear [23:20])
			 */
			reg = mchbar_read32(EPD_99);
			reg = (reg & 0xff0fffff) | ((uint32_t)rank_adj << 20);
			mchbar_write32(EPD_99, reg);
		}

		/* Set constant high-order pattern */
		mchbar_setbits32(EPD_99, 0x55000000);
	}

	/*
	 * EPD+0x2E: set bit 5 for DDR2-533 (mem_clock == 1).
	 * Vendor (fff03005): OR dword [0xfed14a2e], 0x20 - 32-bit unaligned.
	 */
	if (mc == MEM_CLOCK_533MT)
		mchbar_setbits32(EPD_2E, 0x20);

	/*
	 * EPD+0xA0: per-channel rank topology and rank enable.
	 *
	 * The RAMINIT copy at FFF01C47 (dram_optimizations) builds
	 * per-channel rank config bytes at sysinfo+0x43/0x44:
	 *   0x00 = no DIMMs on channel
	 *   0x03 = single-rank DIMM (bits [1:0] = chip selects active)
	 *   0x07 = dual-rank DIMM (bit 2 = dual-rank flag + bits [1:0])
	 *
	 * These are then written to EPD+0xA0 by FFF02A7A:
	 *   bits [2:0]   = ch0 rank config (0x03 or 0x07)
	 *   bits [5:4]   = ch0 rank topology (0x01=single, 0x03=dual)
	 *   bits [10:8]  = ch1 rank config (0x03 or 0x07)
	 *   bits [13:12] = ch1 rank topology (0x01=single, 0x03=dual)
	 *
	 * RAMINIT3 (FFFF4821) does NOT build the rank config bytes.
	 * Without bits [2:0] and [10:8], the EPD address decode
	 * does not enable rank 1, causing data corruption on dual-rank
	 * DIMMs (bit-shift errors).
	 *
	 * BIOS: _DAT_fed14aa0 =
	 *   ((ch1_cfg << 8) | (old & 0xfffff8f8) | ch0_cfg) &
	 *   0xffff0f0f | rank_topology_overlay;
	 */
	{
		int ch0_pop = si->dimms[0].present || si->dimms[1].present;
		int ch1_pop = si->dimms[2].present || si->dimms[3].present;

		if (ch0_pop || ch1_pop) {
			uint32_t rank_topo = 0;
			uint8_t ch0_cfg = 0, ch1_cfg = 0;

			/*
			 * Stepping-dependent rank config override.
			 *
			 * Vendor (FFF01C0D): On non-A0 steppings < 4
			 * (including C0 = stepping 3 on the X61), the
			 * rank config byte is forced to 0x07 for ALL
			 * populated channels regardless of dual-rank.
			 *
			 * The vendor builds the rank config in a separate
			 * function (dram_optimizations, POST FF37) with:
			 *   if (stepping != 0 && stepping < 4):
			 *       if (ch_has_dimm): iVar = 2
			 *   iVar=2 -> ch_cfg = 4|3 = 0x07
			 *
			 * Without this, single-rank DIMMs get ch_cfg=0x03
			 * while the vendor always uses 0x07 on C0.
			 */
			uint8_t stepping = northbridge_stepping();
			int force_cfg = (stepping != 0x00 && stepping < 0x04);

			/* Ch0: rank config byte + topology */
			if (ch0_pop) {
				int ch0_dual = 0;
				for (slot = 0; slot < 2; slot++) {
					if (si->dimms[slot].present
					    && si->dimms[slot].dual_rank)
						ch0_dual = 1;
				}
				ch0_cfg = (ch0_dual || force_cfg) ? 0x07 : 0x03;
				rank_topo |= ch0_dual ? 0x30 : 0x10;
			}

			/* Ch1: rank config byte + topology */
			if (ch1_pop) {
				int ch1_dual = 0;
				for (slot = 2; slot < 4; slot++) {
					if (si->dimms[slot].present
					    && si->dimms[slot].dual_rank)
						ch1_dual = 1;
				}
				ch1_cfg = (ch1_dual || force_cfg) ? 0x07 : 0x03;
				rank_topo |= ch1_dual ? 0x3000 : 0x1000;
			}

		/*
		 * Build EPD+0xA0:
		 *   1. Clear bits [2:0] and [10:8] (rank config)
		 *   2. Write ch0_cfg and ch1_cfg
		 *   3. Clear bits [7:4] and [15:12] (topology)
		 *   4. Write rank topology overlay
		 *   5. Compute upper 16 bits from rank population bitmap
		 */
		reg = mchbar_read32(EPD_A0);
		reg = (reg & 0xfffff8f8) | ch0_cfg
		    | ((uint32_t)ch1_cfg << 8);
		reg = (reg & 0xffff0f0f) | rank_topo;

		/*
		 * Upper 16 bits: rank population count indicator.
		 *
		 * RAMINIT FFF03075-FFF030B2 walks a rank bitmap (sysinfo+0x30,
		 * up to 5 entries) and computes a value shifted into bits [31:16].
		 * The algorithm tracks the bit index of each populated rank:
		 *   - First two populated entries: records the bit index
		 *   - Subsequent entries: increments the recorded index by 1
		 * Then: upper16 = (1 << (result + 1)) << 16.
		 *
		 * For the X61 with 2 dual-rank DIMMs (4 ranks total, bitmap
		 * bits 0-3), the result is 4, giving (1 << 5) << 16 = 0x200000.
		 * Vendor inteltool: EPD_A0 = 0x00203737 (bit 21 set).
		 *
		 * Build the bitmap: one bit per rank, ordered by slot:
		 *   bit 0 = ch0 slot0 rank0, bit 1 = ch0 slot0 rank1,
		 *   bit 2 = ch1 slot0 rank0, bit 3 = ch1 slot0 rank1.
		 */
		{
			uint8_t rank_bitmap = 0;
			int bit = 0;
			for (slot = 0; slot < 4; slot++) {
				if (si->dimms[slot].present) {
					rank_bitmap |= (1 << bit);
					bit++;
					if (si->dimms[slot].dual_rank) {
						rank_bitmap |= (1 << bit);
						bit++;
					}
				}
			}
			if (rank_bitmap)
				reg = (reg & 0xffff) |
				      rank_bitmap_to_upper16(rank_bitmap);
		}

		mchbar_write32(EPD_A0, reg);
		}
	}

	printk(BIOS_DEBUG, "EPD: DRB0=0x%08x DRB1=0x%08x "
	       "DRA0=0x%04x DRA1=0x%04x A0=0x%08x\n",
	       mchbar_read32(EPD_C0DRB01),
	       mchbar_read32(EPD_C1DRB01),
	       mchbar_read16(EPD_C0DRA01),
	       mchbar_read16(EPD_C1DRA01),
	       mchbar_read32(EPD_A0));
}

/*
 * program_channel_population() - Set channel enable bits in EPD
 *
 * Programs EPD+0x2F with per-channel population flags and sets
 * EPD+0x30 bit 26.  Without bit 1 in EPD+0x2F, the memory
 * controller does not route transactions to channel 1.
 *
 * BIOS equivalent: FFF03286 (112 bytes, between POST 0xFF41-0xFF42)
 *
 * This function also restores RCOMP_CFG3 state and handles the
 * D3:F0 cleanup from the JEDEC init phase.
 */
static void program_channel_population(sysinfo_t *si)
{
	/*
	 * EPD+0x2F: set per-channel population bits.
	 *   bit 0 = channel 0 has at least one DIMM
	 *   bit 1 = channel 1 has at least one DIMM
	 *
	 * BIOS (FFF032AB-FFF032CA): checks each DIMM slot presence.
	 */
	uint8_t pop = 0;

	if (si->dimms[0].present || si->dimms[1].present)
		pop |= 1;  /* ch0 populated */
	if (si->dimms[2].present || si->dimms[3].present)
		pop |= 2;  /* ch1 populated */

	/*
	 * Vendor (FFF032B4/FFF032C5): OR dword [0xfed14a2f], 1/2
	 * - 32-bit unaligned writes.  Match vendor access width.
	 */
	if (pop & 1)
		mchbar_setbits32(EPD_2F, 1);
	if (pop & 2)
		mchbar_setbits32(EPD_2F, 2);

	/*
	 * EPD+0x30: set bit 26.
	 * BIOS (FFF032CC): OR dword [0xfed14a30], 0x4000000
	 */
	mchbar_setbits32(EPD_30, 1 << 26);

	printk(BIOS_DEBUG, "EPD channel population: 0x%02x "
	       "(ch0=%d ch1=%d) EPD_30=0x%08x\n",
	       mchbar_read8(EPD_2F), pop & 1, (pop >> 1) & 1,
	       mchbar_read32(EPD_30));
}

/* ================================================================== */
/* DRAM Power Management (POST 0xFF41/0xFF43)                         */
/* ================================================================== */

/*
 * dram_power_mgmt() - Configure DRAM power management and throttling
 *
 * BIOS equivalent: FFF0255E (202 bytes, RAMINIT copy POST 0xFF43)
 * Hardcoded register sequence - no sysinfo inputs.
 *
 * Vendor register sequence (22 operations, no conditionals):
 *   1-2.   Copy TSC1B[6:0] -> TSC0 (per channel) - seed thermal sensor
 *   3-4.   PM_CH_CTRL = 0 (per channel)
 *   5-6.   PM_CH_THRT0: ch0 = 0x17, ch1 = 0
 *   7-8.   PM_CH_THRT1 = 0 (per channel)
 *   9-10.  PM_CH_TSC1 = 0x98 (per channel)
 *   11-12. PM_THRT_MODE = 0x0E, PM_THRT_CTRL = 1
 *   13-14. PM_CH_CMD: ch0 = 0x9200, ch1 = 0
 *   15-16. PM_CH_THRT0 |= 0x80000000 (per channel)
 *   17-18. PM_CH_TSC0 |= 0x80 (per channel)
 *   19-20. PM_CH_TSC1 |= 0x01 (per channel) -> 0x98 becomes 0x99
 *   21-22. CxPWR_THROTTLE1 |= 0x80000000 (per channel)
 */
static void dram_power_mgmt(void)
{
	/*
	 * 1-2: Seed TSC0 from TSC1B with bit 7 cleared.
	 * Vendor reads the thermal sensor readback (TSC1B) and copies
	 * its lower 7 bits into TSC0 before enabling the sensor.
	 */
	mchbar_write8(PM_CH_TSC0(0), mchbar_read8(PM_CH_TSC1B(0)) & 0x7f);
	mchbar_write8(PM_CH_TSC0(1), mchbar_read8(PM_CH_TSC1B(1)) & 0x7f);

	/* 3-4: Clear PM channel control */
	mchbar_write8(PM_CH_CTRL(0), 0);
	mchbar_write8(PM_CH_CTRL(1), 0);

	/* 5-6: PM throttle 0 - ch0 gets base value, ch1 gets 0 */
	mchbar_write32(PM_CH_THRT0(0), 0x17);
	mchbar_write32(PM_CH_THRT0(1), 0);

	/* 7-8: PM throttle 1 - both channels 0 */
	mchbar_write32(PM_CH_THRT1(0), 0);
	mchbar_write32(PM_CH_THRT1(1), 0);

	/* 9-10: Thermal sensor control 1 - base value (bit 0 set later) */
	mchbar_write8(PM_CH_TSC1(0), 0x98);
	mchbar_write8(PM_CH_TSC1(1), 0x98);

	/* 11-12: Global throttle mode and control */
	mchbar_write8(PM_THRT_MODE, 0x0e);
	mchbar_write8(PM_THRT_CTRL, 1);

	/* 13-14: PM command - ch0 active, ch1 idle */
	mchbar_write16(PM_CH_CMD(0), 0x9200);
	mchbar_write16(PM_CH_CMD(1), 0);

	/* 15-16: Enable PM throttle 0 (set bit 31) */
	mchbar_setbits32(PM_CH_THRT0(0), 1 << 31);
	mchbar_setbits32(PM_CH_THRT0(1), 1 << 31);

	/* 17-18: Enable thermal sensor (set bit 7 of TSC0) */
	mchbar_setbits8(PM_CH_TSC0(0), 0x80);
	mchbar_setbits8(PM_CH_TSC0(1), 0x80);

	/* 19-20: Set TSC1 bit 0 (0x98 -> 0x99) */
	mchbar_setbits8(PM_CH_TSC1(0), 1);
	mchbar_setbits8(PM_CH_TSC1(1), 1);

	/* 21-22: Enable power throttle 1 (set bit 31) */
	mchbar_setbits32(CxPWR_THROTTLE1(0), 1 << 31);
	mchbar_setbits32(CxPWR_THROTTLE1(1), 1 << 31);
}

/* ================================================================== */
/* Warm Boot / S3 Resume Detection                                    */
/* ================================================================== */

/*
 * check_warm_boot() - Check SSKPD and PMSTS for warm boot indicator
 *
 * BIOS equivalent: FFFE0F2D (sskpd_check) - checks for 0xCAFE marker
 * If SSKPD contains 0xCAFE, this is a warm boot and we can skip
 * full raminit (just restore timing settings).
 *
 * BIOS equivalent: FFFF5115 checks MCHBAR+0x0F14 (PMSTS)
 *   If PMSTS bits [1:0] == 2, warm reset needed
 *   If PMSTS bit 0 set, self-refresh was maintained
 */
static int check_warm_boot(void)
{
	/* Check SSKPD for magic marker */
	uint16_t sskpd = mchbar_read16(SSKPD_MCHBAR);
	if (sskpd == 0xCAFE)
		return 1;

	/* Check PMSTS for self-refresh state */
	uint32_t pmsts = mchbar_read32(PMSTS_MCHBAR);
	if (pmsts & PMSTS_SELFREFRESH)
		return 1;

	return 0;
}

/*
 * reset_on_stale_rcomp() - Reset if RCOMP engine has stale state
 *
 * Vendor BIOS (FFF00240 / FFFF3220): if the MCH stepping is A0
 * (D0:F0 revision == 0x00) and RCOMP_CTRL bit 1 is set from a
 * prior boot, issue a warm reset (CF9=0x06) to clear MCH state.
 *
 * The vendor guards this with the stepping byte at MMCONFIG
 * D0:F0+0x08 (DAT_f0000008), NOT with PMSTS.  On A1+ steppings
 * (including C0 on the ThinkPad X61), this check never fires.
 */
static void reset_on_stale_rcomp(void)
{
	uint8_t stepping = northbridge_stepping();
	uint32_t rcomp = mchbar_read32(RCOMP_CTRL);

	if (stepping == 0x00 && (rcomp & 2)) {
		printk(BIOS_DEBUG, "raminit: A0 stepping RCOMP_CTRL=0x%08x "
		       "stale - warm reset\n", rcomp);
		system_reset();
	}
}

/*
 * init_pmcon() - Set up ICH8 GEN_PMCON registers at raminit entry
 *
 * Vendor BIOS (FFFF3220, disasm at FFFF3304-FFFF3346):
 *   1. If GEN_PMCON_3 bit 3 (SLP_S3# stretch) set from prior boot:
 *      clear bits 1 (RTC_POWER_FAILED) and 3 (SLP_S3# stretch).
 *   2. GEN_PMCON_2: clear status bits 0,3,4; then set bit 7
 *      ("DRAM init in progress").
 *
 * The bit 7 flag is cleared at POST FF42 when raminit completes.
 * If the system resets mid-init, bit 7 remains set - the ICH8 can
 * use this to distinguish a clean boot from an interrupted one.
 *
 * Vendor does two separate writes to GEN_PMCON_2:
 *   FFFF3304: read, AND 0xE6, write  (clear status bits)
 *   FFFF333D: read, OR  0x80, write  (set DRAM_INIT)
 */
static void init_pmcon(void)
{
	if (pci_read_config8(D31F0, D31F0_GEN_PMCON_3) & GEN_PMCON_3_SLP_S4_STRETCH)
		pci_and_config8(D31F0, D31F0_GEN_PMCON_3,
				(u8)~(RTC_POWER_FAILED | GEN_PMCON_3_SLP_S4_STRETCH));

	/* Clear status bits 0,3,4 (first write) */
	pci_and_config8(D31F0, D31F0_GEN_PMCON_2, GEN_PMCON_2_W1C_BITS);

	/* Set DRAM init in progress (second write) */
	pci_or_config8(D31F0, D31F0_GEN_PMCON_2, GEN_PMCON_2_DRAM_INIT);
}

/*
 * check_bad_warmboot() - POST FF17: Reset if in bad warm boot state
 *
 * Vendor BIOS (FFFF5115, disasm at FFFF5120-FFFF5157):
 *   If PMSTS bits[1:0] == 2 (warm reset without self-refresh):
 *     1. GEN_PMCON_3 |= 0x08  - enable SLP_S3# assertion stretch
 *     2. GEN_PMCON_2 &= 0x7F  - clear DRAM init in progress
 *     3. PMSTS |= 2           - confirm warm reset state
 *     4. full_reset()          - CF9=0x0E (cold reset, never returns)
 *
 * The PMCON manipulation before reset ensures the ICH8 won't
 * assert SLP_S3# prematurely during the reset, and records that
 * DRAM init was not in progress when the reset was issued.
 */
static void check_bad_warmboot(void)
{
	uint32_t pmsts = mchbar_read32(PMSTS_MCHBAR);
	if ((pmsts & 3) != 2)
		return;

	printk(BIOS_DEBUG, "raminit: PMSTS=0x%08x bad warmboot at POST 17"
	       " - PMCON cleanup + full reset\n", pmsts);

	/* SLP_S3# assertion stretch - prevent premature power-down */
	pci_or_config8(D31F0, D31F0_GEN_PMCON_3, GEN_PMCON_3_SLP_S4_STRETCH);

	/* Clear DRAM init in progress */
	pci_and_config8(D31F0, D31F0_GEN_PMCON_2, (u8)~GEN_PMCON_2_DRAM_INIT);

	/* Confirm warm reset state in PMSTS */
	mchbar_setbits32(PMSTS_MCHBAR, PMSTS_WARM_RESET);

	full_reset();
}

/* ================================================================== */
/* Main RAMINIT Entry Point                                           */
/* ================================================================== */

/*
 * raminit() - Full DDR2 memory initialization
 *
 * Orchestrates the complete DRAM init sequence:
 *   1. SPD scan and DIMM detection
 *   2. Frequency and CAS latency selection
 *   3. Timing parameter calculation
 *   4. Clock configuration
 *   5. RCOMP calibration
 *   6. Pre-JEDEC memory map
 *   7. JEDEC DDR2 init sequence
 *   8. Post-JEDEC configuration
 *   9. Final memory map
 *   10. Receive-enable training
 *   11. EPD address decode + channel population (from RAMINIT copy)
 *
 * BIOS POST code progression: 0xFF01 -> 0xFF43
 *
 * BIOS equivalent:
 *   RAMINIT  FFF00240 (main) -> FFF01D4F (sequencer) -> FFF02A7A (EPD)
 *   RAMINIT3 FFFF3220 (main) -> FFFF5115 (sequencer, no EPD)
 *
 * coreboot equivalent: raminit() in raminit.c
 */
void raminit(sysinfo_t *si)
{
	int fast_boot = 0;

	/* Get board-specific SPD address -> DIMM slot mapping */
	mainboard_get_spd_map(si->spd_addr_map);

	/* Check for warm boot / S3 resume */
	si->s3_resume = check_warm_boot();

	reset_on_stale_rcomp();
	init_pmcon();

	/* Attempt fast boot using MRC cache on any boot */
	{
		size_t mrc_size = 0;
		struct gm965_mrc_cache *cached = mrc_cache_current_mmap_leak(MRC_TRAINING_DATA,
									     MRC_CACHE_VERSION,
									     &mrc_size);
		if (cached && mrc_size == sizeof(struct gm965_mrc_cache) &&
		    cached->cpu_id == cpu_get_cpuid()) {
			/* Verify SPD unique CRCs for populated slots */
			int valid = 1;
			for (int slot = 0; slot < 4; slot++) {
				if (cached->dimms[slot].present) {
					u16 crc = spd_read_unique_crc(si->spd_addr_map[slot]);
					if (crc != cached->spd_crc[slot]) {
						valid = 0;
						break;
					}
				}
			}
			if (valid) {
				copy_cache_to_si(si, cached);
				fast_boot = 1;
				printk(BIOS_DEBUG, "%s: fast boot from MRC cache\n", __func__);
			}
		}
	}

	if (!fast_boot) {
		printk(BIOS_DEBUG, "%s: cold boot - scanning DIMMs\n", __func__);
		detect_dimms(si);
		printk(BIOS_DEBUG, "%s: %d DIMM(s), %d channel(s)\n",
		       __func__, si->dimm_count, si->channels);

		printk(BIOS_DEBUG, "%s: selecting frequency and CAS\n", __func__);
		select_frequency_and_cas(si);

		printk(BIOS_DEBUG, "%s: calculating timings\n", __func__);
		calculate_timings(si);
	}

	/*
	 * POST 0xFF17: Bad warm boot check (vendor: FFFF5115).
	 * If PMSTS bits[1:0]==2, clean up GEN_PMCON registers and
	 * issue full reset (CF9=0x0E).  Must run after init_pmcon()
	 * set DRAM_INIT but before the sequencer steps begin.
	 */
	check_bad_warmboot();

	/* PMSTS bit 0: mark raminit in progress (vendor: FFFF515D) */
	mchbar_setbits32(PMSTS_MCHBAR, PMSTS_SELFREFRESH);

	/*
	 * POST 0xFF18: Program DRAM frequency in CLKCFG and relock PLL.
	 * BIOS: FFFF4DF7 (121 bytes) + FFF009AD (PLL) + FFF00A8E (UPDATE toggle)
	 */
	program_clkcfg_lock(si);

	/*
	 * POST 0xFF19: IGD GCFGC render/sampler clock setup
	 * BIOS: FFFF3844 - programs D2F0+0xF0 (GCFGC), NOT CLKCFG
	 * CLKCFG was already programmed inline at POST 0xFF18 above.
	 */
	printk(BIOS_DEBUG, "%s: POST 19 - program_gcfgc\n", __func__);
	program_gcfgc(si);

	printk(BIOS_DEBUG, "%s: POST 20 - set_clkcross_frequencies\n", __func__);
	set_clkcross_frequencies(si);

	printk(BIOS_DEBUG, "%s: POST 21 - pre-JEDEC MCHBAR regs\n", __func__);
	mchbar_setbits32(FSBPMC3, 1 << 1);
	mchbar_setbits32(SBTEST, 6);
	mchbar_setbits32(POST_JEDEC_TIM0, 0x3000000);
	mchbar_setbits32(POST_JEDEC_TIM1, 0x3000000);

	printk(BIOS_DEBUG, "%s: POST 22 - prejedec_memory_map\n", __func__);
	if (!si->s3_resume)
		prejedec_memory_map(si);

	printk(BIOS_DEBUG, "%s: POST 23 - rcomp_init\n", __func__);
	rcomp_init(si);

	printk(BIOS_DEBUG, "%s: POST 24 - odt_pre_setup\n", __func__);
	odt_pre_setup(si);

	printk(BIOS_DEBUG, "%s: POST 25 - program_timings\n", __func__);
	program_timings(si);
	program_dram_control(si);

	printk(BIOS_DEBUG, "%s: POST 26 - misc_odt_settings\n", __func__);
	misc_odt_settings(si);

	printk(BIOS_DEBUG, "%s: POST 27 - ddr2_odt_setup\n", __func__);
	ddr2_odt_setup(si);

	printk(BIOS_DEBUG, "%s: POST 28 - ddr2_memory_io_init_phase1\n", __func__);
	ddr2_memory_io_init_phase1(si);

	printk(BIOS_DEBUG, "%s: POST 29 - ddr2_memory_io_init_phase2\n", __func__);
	ddr2_memory_io_init_phase2(si);

	printk(BIOS_DEBUG, "%s: POST 30 - program_rw_pointer_and_ddr_type\n", __func__);
	program_rw_pointer_and_ddr_type(si);

	printk(BIOS_DEBUG, "%s: POST 31 - enable_dram_clocks\n", __func__);
	enable_dram_clocks(si);

	/*
	 * POST 0xFF32: Wait for RCOMP calibration to complete.
	 *
	 * Vendor BIOS (FFFF5115): polls RCOMP_CTRL bit 0 in an infinite
	 * loop - no timeout.  If RCOMP never completes the system hangs
	 * here, which is correct: proceeding with uncalibrated DRAM I/O
	 * impedances causes silent data corruption.
	 *
	 * Vendor guard: stepping != A1 (DAT_f0000008 != 0x01).
	 * On A1 stepping only the RCOMP wait is skipped.
	 * A0 and C0 always wait regardless of cold/warm boot.
	 */
	printk(BIOS_DEBUG, "%s: POST 32 - waiting for RCOMP "
	       "(RCOMP_CTRL=0x%08x)\n", __func__, mchbar_read32(RCOMP_CTRL));
	if (northbridge_stepping() != 0x01) {
		while (mchbar_read32(RCOMP_CTRL) & 1)
			;
		printk(BIOS_DEBUG, "%s: RCOMP done "
		       "(RCOMP_CTRL=0x%08x)\n",
		       __func__, mchbar_read32(RCOMP_CTRL));
	}

	/*
	 * RCOMP teardown (vendor FFFF52BD):
	 *   RCOMP_CFG3 &= ~(1<<17)          - clear comparator arm bit
	 *   RCOMP_CTRL &= 0xfffcffcf        - clear bits 17,16,5,4
	 * CFG3 must be cleared before CTRL.
	 */
	mchbar_clrbits32(RCOMP_CFG3, 1 << 17);
	mchbar_clrbits32(RCOMP_CTRL, (3 << 16) | (3 << 4));

	/*
	 * POST 0xFF33: JEDEC initialization.
	 *
	 * RAMINIT has two JEDEC init functions - only ONE runs per boot:
	 *   Cold boot (sysinfo[0x40]=0): FFF0194F runs for all ranks.
	 *     FFF030BD is SKIPPED.
	 *   Warm boot (sysinfo[0x40]=1): FFF030BD runs.
	 *     FFF0194F is SKIPPED (EDI=1 from caller).
	 *
	 * FFF0194F (cold boot path, 630 bytes) sends the standard DDR2
	 * JEDEC MRS sequence per rank with computed timing parameters.
	 * It uses a ROM table at FFF03B98 containing 8 pointers to
	 * 16-bit DRB entries for rank addressing - identical to our
	 * get_rank_addr() which reads the same DRB registers.
	 *
	 * Our jedec_init_ddr2() is equivalent to FFF0194F: it sends
	 * the full MRS sequence (NOP -> ABP -> EMRS2 -> EMRS3 ->
	 * EMRS1(ODT) -> MRS(DLL reset) -> ABP -> CBRx2 -> MRS(normal) ->
	 * EMRS1(OCD cal) -> EMRS1(exit)) per populated rank using
	 * SPD-derived timing parameters.
	 */
	printk(BIOS_DEBUG, "%s: POST 33 - jedec_init_ddr2\n", __func__);
	if (!si->s3_resume)
		jedec_init_ddr2(si);

	printk(BIOS_DEBUG, "%s: POST 34 - post_jedec_sequence\n", __func__);
	post_jedec_sequence(si);

	printk(BIOS_DEBUG, "%s: POST 35 - program_memory_map\n", __func__);
	program_memory_map(si, igd_compute_ggc());

	printk(BIOS_DEBUG, "%s: POST 36 - final_timing_adjust\n", __func__);
	final_timing_adjust(si);

	printk(BIOS_DEBUG, "%s: POST 37 - dram_optimizations\n", __func__);
	dram_optimizations(si);

	printk(BIOS_DEBUG, "%s: POST 38 - DCC final\n", __func__);
	mchbar_setbits32(DCC_MCHBAR, 0xf8000);

	printk(BIOS_DEBUG, "%s: POST 39 - receive_enable_training "
	       "(fast_boot=%d)\n", __func__, fast_boot);
	if (fast_boot) {
		raminit_program_training(si);
	} else {
		receive_enable_training(si);
	}
	printk(BIOS_DEBUG, "%s: POST 39 done\n", __func__);

	/*
	 * POST 0xFF40: RCOMP lock and clock restore.
	 *
	 * Vendor BIOS (FFFF5115, FFFF5377):
	 *   1. If stepping != A0: set RCOMP_CTRL bit 1 (RCOMP_LOCK).
	 *   2. Clear MCHBAR[0x1444] bit 12 (disable RCOMP engine clock).
	 *      This is the reverse of the enable done at the start of ff23.
	 *      Vendor trace: [0x1444] <= 0x00000115 (bit 12 cleared).
	 */
	if (northbridge_stepping() != 0x00)
		mchbar_setbits8(RCOMP_CTRL, 2);
	mchbar_clrbits32(IO_RCOMP_CLK_EN, 1 << 12);

	/*
	 * POST 0xFF41: EPD address decode and channel population.
	 *
	 * The RAMINIT copy at FFF0xxxx (the BIOS's actual execution
	 * path, 55 functions) programs the EPD register region
	 * (MCHBAR 0x0A00-0x0AA0) between POST 0xFF41 and 0xFF42.
	 * The RAMINIT3 copy at FFFF3xxx (29 functions) that the rest
	 * of this file was originally based on omits these entirely.
	 *
	 * Without EPD programming, the memory controller does not
	 * know how to decode addresses for channel 1.
	 *
	 * BIOS flow (FFF01F8A-FFF01FDE):
	 *   1. RCOMP lock (done above at POST 0xFF40)
	 *   2. IO_RCOMP_CLK_EN clear (done above)
	 *   3. If EPD+0x2E capability set:
	 *        a. program_epd() - FFF02A7A (1603 bytes)
	 *   4. program_channel_population() - FFF03286 (112 bytes)
	 */
	/*
	 * EPD gating: match vendor boot-state detection (FFF0287C).
	 *
	 * The vendor reads EPD_2E[4:0] at raminit entry and caches
	 * it in sysinfo[0x30].  On the very first cold boot (power-on),
	 * EPD_2E is 0 (hardware default - never programmed).  The
	 * vendor uses this to SKIP both program_epd() and
	 * program_channel_population() on the first cold boot.
	 *
	 * Memory still works because the primary DRB/DRA/DCC registers
	 * are sufficient for basic rank addressing.  EPD provides a
	 * secondary scheduling/decode path that the MC latches from its
	 * registers; programming it with even slightly wrong values can
	 * actively break rank addressing that works with HW defaults.
	 *
	 * On subsequent boots (warm reset, S3 resume), EPD_2E
	 * retains its value from the prior successful boot, so
	 * sysinfo[0x30] != 0 and both EPD functions run.
	 *
	 * Vendor sequence (FFF01D4F at POST 0xFF41):
	 *   if (sysinfo[0x30] != 0):
	 *       if (cold_boot): program_epd()
	 *       program_channel_population()
	 */
	{
		uint8_t epd_state = mchbar_read8(EPD_2E) & 0x1f;

		printk(BIOS_DEBUG, "%s: POST 41 - EPD_2E=0x%02x\n",
		       __func__, epd_state);

		if (epd_state != 0) {
			printk(BIOS_DEBUG, "%s: EPD address decode "
			       "(prior init detected)\n", __func__);
			if (!si->s3_resume)
				program_epd(si);
			program_channel_population(si);

			/*
			 * DCC2 fixup: read EPD_2E[4:0] and write to DCC2.
			 *
			 * Vendor (FFF028B6/FFF01378): DCC2 stores a channel
			 * address decode value derived from EPD_2E[4:0].
			 * These bits are latched by the MC hardware after
			 * the EPD DRB copies are programmed.
			 *
			 * Inteltool confirms: EPD_2E = 0x10, DCC2 = 0x10
			 * (ch0 rank0 boundary in 32MB units for 1GB DR DIMM).
			 */
			uint8_t dcc2_val = mchbar_read8(EPD_2E) & 0x1f;
			mchbar_write16(DCC2_MCHBAR, dcc2_val);
			printk(BIOS_DEBUG, "%s: DCC2 = 0x%02x\n",
			       __func__, dcc2_val);
		} else {
			printk(BIOS_DEBUG, "%s: EPD skipped "
			       "(first cold boot - HW defaults)\n", __func__);
		}
	}

	/*
	 * POST 0xFF42: Clear GEN_PMCON_2 DRAM_INIT bit
	 *
	 * Vendor BIOS (FFFF5115, FFFF53A4): GEN_PMCON_2 &= 0x7F
	 * Clears "DRAM init in progress" - signals raminit completed
	 * successfully.  Matches the set in init_pmcon() at entry.
	 */
	pci_and_config8(D31F0, D31F0_GEN_PMCON_2, (u8)~GEN_PMCON_2_DRAM_INIT);

	/*
	 * POST 0xFF43: DRAM power management, thermal init, final cleanup
	 * BIOS: FFF0255E (RAMINIT) / FFFF53D9 (RAMINIT3)
	 */
	printk(BIOS_DEBUG, "%s: POST 43 - dram_power_mgmt\n", __func__);
	dram_power_mgmt();
	printk(BIOS_DEBUG, "%s: thermal_init\n", __func__);
	gm965_thermal_init(si);

	/*
	 * final_cleanup() is NOT called here.
	 *
	 * The RAMINIT copy (FFF02838) has zero callers in the vendor
	 * BIOS image - confirmed dead code.  The MCHBAR dump confirms:
	 * vendor has 0x0ffc=0x01000000 (bit 23 clear) and no IOSCHED
	 * registers (0x1120-112c, 0x11b0) programmed.  Calling
	 * final_cleanup() sets bit 23 of 0x0ffc and writes IOSCHED
	 * values that the vendor never writes from the RAMINIT path.
	 */

	mchbar_write32(SSKPD_MCHBAR, 0xCAFE);

	/* Expose fast_boot to romstage so it can stash MRC data after cbmem_recovery() */
	si->fast_boot = fast_boot;

	printk(BIOS_DEBUG, "%s: complete\n", __func__);
}

/*
 * raminit_stash_mrc_cache() - Build MRC cache struct and stash to CBMEM.
 *
 * Must be called after cbmem_recovery() so CBMEM is available.
 * Only called on cold boot (!si->fast_boot) - on fast boot the cache
 * is already valid and there's nothing new to save.
 */
void raminit_stash_mrc_cache(const sysinfo_t *si)
{
	struct gm965_mrc_cache cache;

	memset(&cache, 0, sizeof(cache));

	cache.cpu_id = cpu_get_cpuid();

	/* Store SPD unique CRCs for cache validation on next boot */
	for (int slot = 0; slot < 4; slot++) {
		if (si->dimms[slot].present)
			cache.spd_crc[slot] = spd_read_unique_crc(si->spd_addr_map[slot]);
	}

	memcpy(cache.dimms, si->dimms, sizeof(cache.dimms));
	memcpy(&cache.timings, &si->timings, sizeof(cache.timings));
	memcpy(cache.rec_coarse, si->rec_coarse, sizeof(cache.rec_coarse));
	memcpy(cache.rec_coarse_low, si->rec_coarse_low, sizeof(cache.rec_coarse_low));
	memcpy(cache.rec_fine, si->rec_fine, sizeof(cache.rec_fine));
	memcpy(cache.raw_spd, si->raw_spd, sizeof(cache.raw_spd));

	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION,
			     &cache, sizeof(cache));

	printk(BIOS_DEBUG, "GM965 MRC cache stashed (%zu bytes)\n", sizeof(cache));
}
