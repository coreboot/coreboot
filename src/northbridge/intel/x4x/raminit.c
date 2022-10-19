/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <arch/cpu.h>
#include <spd.h>
#include <string.h>
#include <device/dram/ddr2.h>
#include <device/dram/ddr3.h>
#include <mrc_cache.h>
#include <timestamp.h>
#include <types.h>

#include "raminit.h"
#include "x4x.h"

#define MRC_CACHE_VERSION 0

static u16 ddr2_get_crc(u8 device, u8 len)
{
	u8 raw_spd[128] = {};
	i2c_eeprom_read(device, 64, 9, &raw_spd[64]);
	i2c_eeprom_read(device, 93, 6, &raw_spd[93]);
	return spd_ddr2_calc_unique_crc(raw_spd, len);
}

static u16 ddr3_get_crc(u8 device, u8 len)
{
	u8 raw_spd[256] = {};
	i2c_eeprom_read(device, 117, 11, &raw_spd[117]);
	return spd_ddr3_calc_unique_crc(raw_spd, len);
}

static enum cb_err verify_spds(const u8 *spd_map, const struct sysinfo *ctrl_cached)
{
	int i;
	u16 crc;

	for (i = 0; i < TOTAL_DIMMS; i++) {
		if (!(spd_map[i]))
			continue;
		int len = smbus_read_byte(spd_map[i], 0);
		if (len < 0 && ctrl_cached->dimms[i].card_type == RAW_CARD_UNPOPULATED)
			continue;
		if (len > 0 && ctrl_cached->dimms[i].card_type == RAW_CARD_UNPOPULATED)
			return CB_ERR;

		if (ctrl_cached->spd_type == DDR2)
			crc = ddr2_get_crc(spd_map[i], len);
		else
			crc = ddr3_get_crc(spd_map[i], len);

		if (crc != ctrl_cached->dimms[i].spd_crc)
			return CB_ERR;
	}
	return CB_SUCCESS;
}

struct abs_timings {
	u32 min_tclk;
	u32 min_tRAS;
	u32 min_tRP;
	u32 min_tRCD;
	u32 min_tWR;
	u32 min_tRFC;
	u32 min_tWTR;
	u32 min_tRRD;
	u32 min_tRTP;
	u32 min_tAA;
	u32 min_tCLK_cas[8];
	u32 cas_supported;
};

#define CTRL_MIN_TCLK_DDR2 TCK_400MHZ

static void select_cas_dramfreq_ddr2(struct sysinfo *s, const struct abs_timings *saved_timings)
{
	u8 try_cas;
	/* Currently only these CAS are supported */
	u8 cas_mask = SPD_CAS_LATENCY_DDR2_5 | SPD_CAS_LATENCY_DDR2_6;

	cas_mask &= saved_timings->cas_supported;
	try_cas = spd_get_msbs(cas_mask);

	while (cas_mask & (1 << try_cas) && try_cas > 0) {
		s->selected_timings.CAS = try_cas;
		s->selected_timings.tclk = saved_timings->min_tCLK_cas[try_cas];
		if (s->selected_timings.tclk >= CTRL_MIN_TCLK_DDR2 &&
				saved_timings->min_tCLK_cas[try_cas] !=
				saved_timings->min_tCLK_cas[try_cas - 1])
			break;
		try_cas--;
	}

	if ((s->selected_timings.CAS < 3) || (s->selected_timings.tclk == 0))
		die("Could not find common memory frequency and CAS\n");

	switch (s->selected_timings.tclk) {
	case TCK_200MHZ:
	case TCK_266MHZ:
		/* FIXME: this works on vendor BIOS */
		die("Selected dram frequency not supported\n");
	case TCK_333MHZ:
		s->selected_timings.mem_clk = MEM_CLOCK_667MHz;
		break;
	case TCK_400MHZ:
		s->selected_timings.mem_clk = MEM_CLOCK_800MHz;
		break;
	}
}

static void mchinfo_ddr2(struct sysinfo *s)
{
	const u32 eax = cpuid_ext(0x04, 0).eax;
	printk(BIOS_WARNING, "%d CPU cores\n", ((eax >> 26) & 0x3f) + 1);

	u32 capid = pci_read_config16(HOST_BRIDGE, 0xe8);
	if (!(capid & (1<<(79-64))))
		printk(BIOS_WARNING, "iTPM enabled\n");

	capid = pci_read_config32(HOST_BRIDGE, 0xe4);
	if (!(capid & (1<<(57-32))))
		printk(BIOS_WARNING, "ME enabled\n");

	if (!(capid & (1<<(56-32))))
		printk(BIOS_WARNING, "AMT enabled\n");

	if (!(capid & (1<<(48-32))))
		printk(BIOS_WARNING, "VT-d enabled\n");
}

static enum cb_err ddr2_save_dimminfo(u8 dimm_idx, u8 *raw_spd,
		struct abs_timings *saved_timings, struct sysinfo *s)
{
	struct dimm_attr_ddr2_st decoded_dimm;
	int i;

	if (spd_decode_ddr2(&decoded_dimm, raw_spd) != SPD_STATUS_OK) {
		printk(BIOS_DEBUG, "Problems decoding SPD\n");
		return CB_ERR;
	}

	if (CONFIG(DEBUG_RAM_SETUP))
		dram_print_spd_ddr2(&decoded_dimm);

	if (!(decoded_dimm.width & (0x08 | 0x10))) {

		printk(BIOS_ERR, "DIMM%d Unsupported width: x%d. Disabling dimm\n",
			dimm_idx, s->dimms[dimm_idx].width);
		return CB_ERR;
	}
	s->dimms[dimm_idx].width = (decoded_dimm.width >> 3) - 1;
	/*
	 * This boils down to:
	 * "Except for the x16 configuration, all DDR2 devices have a
	 * 1KB page size. For the x16 configuration, the page size is 2KB
	 * for all densities except the 256Mb device, which has a 1KB page
	 * size." Micron, 'TN-47-16 Designing for High-Density DDR2 Memory'
	 * The formula is pagesize in KiB = width * 2^col_bits / 8.
	 */
	s->dimms[dimm_idx].page_size = decoded_dimm.width * (1 << decoded_dimm.col_bits) / 8;

	switch (decoded_dimm.banks) {
	case 4:
		s->dimms[dimm_idx].n_banks = N_BANKS_4;
		break;
	case 8:
		s->dimms[dimm_idx].n_banks = N_BANKS_8;
		break;
	default:
		printk(BIOS_ERR, "DIMM%d Unsupported #banks: x%d. Disabling dimm\n",
			 dimm_idx, decoded_dimm.banks);
		return CB_ERR;
	}

	s->dimms[dimm_idx].ranks = decoded_dimm.ranks;
	s->dimms[dimm_idx].rows = decoded_dimm.row_bits;
	s->dimms[dimm_idx].cols = decoded_dimm.col_bits;

	saved_timings->cas_supported &= decoded_dimm.cas_supported;

	saved_timings->min_tRAS = MAX(saved_timings->min_tRAS, decoded_dimm.tRAS);
	saved_timings->min_tRP  = MAX(saved_timings->min_tRP,  decoded_dimm.tRP);
	saved_timings->min_tRCD = MAX(saved_timings->min_tRCD, decoded_dimm.tRCD);
	saved_timings->min_tWR  = MAX(saved_timings->min_tWR,  decoded_dimm.tWR);
	saved_timings->min_tRFC = MAX(saved_timings->min_tRFC, decoded_dimm.tRFC);
	saved_timings->min_tWTR = MAX(saved_timings->min_tWTR, decoded_dimm.tWTR);
	saved_timings->min_tRRD = MAX(saved_timings->min_tRRD, decoded_dimm.tRRD);
	saved_timings->min_tRTP = MAX(saved_timings->min_tRTP, decoded_dimm.tRTP);
	for (i = 0; i < 8; i++) {
		if (!(saved_timings->cas_supported & (1 << i)))
			saved_timings->min_tCLK_cas[i] = 0;
		else
			saved_timings->min_tCLK_cas[i] =
				MAX(saved_timings->min_tCLK_cas[i],
					decoded_dimm.cycle_time[i]);
	}

	s->dimms[dimm_idx].spd_crc = spd_ddr2_calc_unique_crc(raw_spd,
					spd_decode_spd_size_ddr2(raw_spd[0]));
	return CB_SUCCESS;
}

static void normalize_tCLK(u32 *tCLK)
{
	if (*tCLK <= TCK_666MHZ)
		*tCLK = TCK_666MHZ;
	else if (*tCLK <= TCK_533MHZ)
		*tCLK = TCK_533MHZ;
	else if (*tCLK <= TCK_400MHZ)
		*tCLK = TCK_400MHZ;
	else
		*tCLK = 0;
}

static void select_cas_dramfreq_ddr3(struct sysinfo *s, struct abs_timings *saved_timings)
{
	/*
	 * various constraints must be fulfilled:
	 *  CAS * tCK < 20ns == 160MTB
	 * tCK_max >= tCK >= tCK_min
	 * CAS >= roundup(tAA_min/tCK)
	 * CAS supported
	 * AND BTW: Clock(MT) = 2000 / tCK(ns) - intel uses MTs but calls them MHz
	 */

	u32 min_tCLK;
	u8 try_CAS;
	u16 capid = (pci_read_config16(HOST_BRIDGE, 0xea) >> 4) & 0x3f;

	switch (s->max_fsb) {
	default:
	case FSB_CLOCK_800MHz:
		min_tCLK = TCK_400MHZ;
		break;
	case FSB_CLOCK_1066MHz:
		min_tCLK = TCK_533MHZ;
		break;
	case FSB_CLOCK_1333MHz:
		min_tCLK = TCK_666MHZ;
		break;
	}

	switch (capid >> 3) {
	default: /* Should not happen */
		min_tCLK = TCK_400MHZ;
		break;
	case 1:
		min_tCLK = MAX(min_tCLK, TCK_400MHZ);
		break;
	case 2:
		min_tCLK = MAX(min_tCLK, TCK_533MHZ);
		break;
	case 3: /* Only on P45 */
	case 0:
		min_tCLK = MAX(min_tCLK, TCK_666MHZ);
		break;
	}

	min_tCLK = MAX(min_tCLK, saved_timings->min_tclk);
	if (min_tCLK == 0) {
		printk(BIOS_ERR,
			"DRAM frequency is under lowest supported frequency (400 MHz).\n"
			"Increasing to 400 MHz as last resort.\n");
		min_tCLK = TCK_400MHZ;
	}

	while (1) {
		normalize_tCLK(&min_tCLK);
		if (min_tCLK == 0)
			die("Couldn't find compatible clock / CAS settings.\n");
		try_CAS = DIV_ROUND_UP(saved_timings->min_tAA, min_tCLK);
		printk(BIOS_SPEW, "Trying CAS %u, tCK %u.\n", try_CAS, min_tCLK);
		for (; try_CAS <= DDR3_MAX_CAS; try_CAS++) {
			/*
			 * cas_supported is encoded like the SPD which starts
			 * at CAS=4.
			 */
			if ((saved_timings->cas_supported << 4) & (1 << try_CAS))
				break;
		}
		if ((try_CAS <= DDR3_MAX_CAS) && (try_CAS * min_tCLK < 20 * 256)) {
			/* Found good CAS. */
			printk(BIOS_SPEW, "Found compatible tCLK / CAS pair: %u / %u.\n",
				min_tCLK, try_CAS);
			break;
		}
		/*
		 * If no valid tCLK / CAS pair could be found for a tCLK
		 * increase it after which it gets normalised. This means
		 * that a lower frequency gets tried.
		 */
		min_tCLK++;
	}

	s->selected_timings.tclk = min_tCLK;
	s->selected_timings.CAS = try_CAS;

	switch (s->selected_timings.tclk) {
	case TCK_400MHZ:
		s->selected_timings.mem_clk = MEM_CLOCK_800MHz;
		break;
	case TCK_533MHZ:
		s->selected_timings.mem_clk = MEM_CLOCK_1066MHz;
		break;
	case TCK_666MHZ:
		s->selected_timings.mem_clk = MEM_CLOCK_1333MHz;
		break;
	}
}

/* With DDR3 and 533MHz mem clock and an enabled internal gfx device the display
   is not usable in non stacked mode, so select stacked mode accordingly */
static void workaround_stacked_mode(struct sysinfo *s)
{
	u32 deven;
	/* Only a problem on DDR3 */
	if (s->spd_type == DDR2)
		return;
	/* Does not matter if only one channel is populated */
	if (!CHANNEL_IS_POPULATED(s->dimms, 0) || !CHANNEL_IS_POPULATED(s->dimms, 1))
		return;
	if (s->selected_timings.mem_clk != MEM_CLOCK_1066MHz)
		return;
	/* IGD0EN gets disabled if not present before this code runs */
	deven = pci_read_config32(HOST_BRIDGE, D0F0_DEVEN);
	if (deven & IGD0EN)
		s->stacked_mode = 1;
}

static enum cb_err ddr3_save_dimminfo(u8 dimm_idx, u8 *raw_spd,
		struct abs_timings *saved_timings, struct sysinfo *s)
{
	struct dimm_attr_ddr3_st decoded_dimm;

	if (spd_decode_ddr3(&decoded_dimm, raw_spd) != SPD_STATUS_OK)
		return CB_ERR;

	if (CONFIG(DEBUG_RAM_SETUP))
		dram_print_spd_ddr3(&decoded_dimm);

	/* x4 DIMMs are not supported (true for both ddr2 and ddr3) */
	if (!(decoded_dimm.width & (0x8 | 0x10))) {
		printk(BIOS_ERR, "DIMM%d Unsupported width: x%d. Disabling dimm\n",
			dimm_idx, s->dimms[dimm_idx].width);
		return CB_ERR;
	}
	s->dimms[dimm_idx].width = (decoded_dimm.width >> 3) - 1;
	/*
	 * This boils down to:
	 * "Except for the x16 configuration, all DDR3 devices have a
	 * 1KB page size. For the x16 configuration, the page size is 2KB
	 * for all densities except the 256Mb device, which has a 1KB page size."
	 * Micron, 'TN-47-16 Designing for High-Density DDR2 Memory'
	*/
	s->dimms[dimm_idx].page_size = decoded_dimm.width * (1 << decoded_dimm.col_bits) / 8;

	s->dimms[dimm_idx].n_banks = N_BANKS_8; /* Always 8 banks on ddr3?? */

	s->dimms[dimm_idx].ranks = decoded_dimm.ranks;
	s->dimms[dimm_idx].rows = decoded_dimm.row_bits;
	s->dimms[dimm_idx].cols = decoded_dimm.col_bits;

	saved_timings->min_tRAS = MAX(saved_timings->min_tRAS, decoded_dimm.tRAS);
	saved_timings->min_tRP  = MAX(saved_timings->min_tRP,  decoded_dimm.tRP);
	saved_timings->min_tRCD = MAX(saved_timings->min_tRCD, decoded_dimm.tRCD);
	saved_timings->min_tWR  = MAX(saved_timings->min_tWR,  decoded_dimm.tWR);
	saved_timings->min_tRFC = MAX(saved_timings->min_tRFC, decoded_dimm.tRFC);
	saved_timings->min_tWTR = MAX(saved_timings->min_tWTR, decoded_dimm.tWTR);
	saved_timings->min_tRRD = MAX(saved_timings->min_tRRD, decoded_dimm.tRRD);
	saved_timings->min_tRTP = MAX(saved_timings->min_tRTP, decoded_dimm.tRTP);
	saved_timings->min_tAA  = MAX(saved_timings->min_tAA,  decoded_dimm.tAA);
	saved_timings->cas_supported &= decoded_dimm.cas_supported;

	s->dimms[dimm_idx].spd_crc = spd_ddr3_calc_unique_crc(raw_spd, raw_spd[0]);

	s->dimms[dimm_idx].mirrored = decoded_dimm.flags.pins_mirrored;

	return CB_SUCCESS;
}

static void select_discrete_timings(struct sysinfo *s, const struct abs_timings *timings)
{
	s->selected_timings.tRAS = DIV_ROUND_UP(timings->min_tRAS, s->selected_timings.tclk);
	s->selected_timings.tRP  = DIV_ROUND_UP(timings->min_tRP,  s->selected_timings.tclk);
	s->selected_timings.tRCD = DIV_ROUND_UP(timings->min_tRCD, s->selected_timings.tclk);
	s->selected_timings.tWR  = DIV_ROUND_UP(timings->min_tWR,  s->selected_timings.tclk);
	s->selected_timings.tRFC = DIV_ROUND_UP(timings->min_tRFC, s->selected_timings.tclk);
	s->selected_timings.tWTR = DIV_ROUND_UP(timings->min_tWTR, s->selected_timings.tclk);
	s->selected_timings.tRRD = DIV_ROUND_UP(timings->min_tRRD, s->selected_timings.tclk);
	s->selected_timings.tRTP = DIV_ROUND_UP(timings->min_tRTP, s->selected_timings.tclk);
}
static void print_selected_timings(struct sysinfo *s)
{
	printk(BIOS_DEBUG, "Selected timings:\n");
	printk(BIOS_DEBUG, "\tFSB:  %dMHz\n", fsb_to_mhz(s->selected_timings.fsb_clk));
	printk(BIOS_DEBUG, "\tDDR:  %dMHz\n", ddr_to_mhz(s->selected_timings.mem_clk));

	printk(BIOS_DEBUG, "\tCAS:  %d\n", s->selected_timings.CAS);
	printk(BIOS_DEBUG, "\ttRAS: %d\n", s->selected_timings.tRAS);
	printk(BIOS_DEBUG, "\ttRP:  %d\n", s->selected_timings.tRP);
	printk(BIOS_DEBUG, "\ttRCD: %d\n", s->selected_timings.tRCD);
	printk(BIOS_DEBUG, "\ttWR:  %d\n", s->selected_timings.tWR);
	printk(BIOS_DEBUG, "\ttRFC: %d\n", s->selected_timings.tRFC);
	printk(BIOS_DEBUG, "\ttWTR: %d\n", s->selected_timings.tWTR);
	printk(BIOS_DEBUG, "\ttRRD: %d\n", s->selected_timings.tRRD);
	printk(BIOS_DEBUG, "\ttRTP: %d\n", s->selected_timings.tRTP);
}

static void find_fsb_speed(struct sysinfo *s)
{
	switch ((mchbar_read32(CLKCFG_MCHBAR) & CLKCFG_FSBCLK_MASK) >> CLKCFG_FSBCLK_SHIFT) {
	case 0x0:
		s->max_fsb = FSB_CLOCK_1066MHz;
		break;
	case 0x2:
		s->max_fsb = FSB_CLOCK_800MHz;
		break;
	case 0x4:
		s->max_fsb = FSB_CLOCK_1333MHz;
		break;
	default:
		s->max_fsb = FSB_CLOCK_800MHz;
		printk(BIOS_WARNING, "Can't detect FSB, setting 800MHz\n");
		break;
	}
	s->selected_timings.fsb_clk = s->max_fsb;
}

static void decode_spd_select_timings(struct sysinfo *s)
{
	unsigned int device;
	u8 dram_type_mask = (1 << DDR2) | (1 << DDR3);
	u8 dimm_mask = 0;
	u8 raw_spd[256];
	int i, j;
	struct abs_timings saved_timings;
	memset(&saved_timings, 0, sizeof(saved_timings));
	saved_timings.cas_supported = UINT32_MAX;

	FOR_EACH_DIMM(i) {
		s->dimms[i].card_type = RAW_CARD_POPULATED;
		device = s->spd_map[i];
		if (!device) {
			s->dimms[i].card_type = RAW_CARD_UNPOPULATED;
			continue;
		}
		switch (smbus_read_byte(s->spd_map[i], SPD_MEMORY_TYPE)) {
		case DDR2SPD:
			dram_type_mask &= 1 << DDR2;
			s->spd_type = DDR2;
			break;
		case DDR3SPD:
			dram_type_mask &= 1 << DDR3;
			s->spd_type = DDR3;
			break;
		default:
			s->dimms[i].card_type = RAW_CARD_UNPOPULATED;
			continue;
		}
		if (!dram_type_mask)
			die("Mixing up dimm types is not supported!\n");

		printk(BIOS_DEBUG, "Decoding dimm %d\n", i);
		if (i2c_eeprom_read(device, 0, 128, raw_spd) != 128) {
			printk(BIOS_DEBUG,
				"i2c block operation failed, trying smbus byte operation.\n");
			for (j = 0; j < 128; j++)
				raw_spd[j] = smbus_read_byte(device, j);
		}

		if (s->spd_type == DDR2){
			if (ddr2_save_dimminfo(i, raw_spd, &saved_timings, s)) {
				printk(BIOS_WARNING,
					"Encountered problems with SPD, skipping this DIMM.\n");
				s->dimms[i].card_type = RAW_CARD_UNPOPULATED;
				continue;
			}
		} else { /* DDR3 */
			if (ddr3_save_dimminfo(i, raw_spd, &saved_timings, s)) {
				printk(BIOS_WARNING,
					"Encountered problems with SPD, skipping this DIMM.\n");
				/* something in decoded SPD was unsupported */
				s->dimms[i].card_type = RAW_CARD_UNPOPULATED;
				continue;
			}
		}
		dimm_mask |= (1 << i);
	}
	if (!dimm_mask)
		die("No memory installed.\n");

	if (s->spd_type == DDR2)
		select_cas_dramfreq_ddr2(s, &saved_timings);
	else
		select_cas_dramfreq_ddr3(s, &saved_timings);
	select_discrete_timings(s, &saved_timings);
	workaround_stacked_mode(s);
}

static void find_dimm_config(struct sysinfo *s)
{
	int chan, i;

	FOR_EACH_POPULATED_CHANNEL(s->dimms, chan) {
		FOR_EACH_POPULATED_DIMM_IN_CHANNEL(s->dimms, chan, i) {
			int dimm_config;
			if (s->dimms[i].ranks == 1) {
				if (s->dimms[i].width == 0)	/* x8 */
					dimm_config = 1;
				else				/* x16 */
					dimm_config = 3;
			} else {
				if (s->dimms[i].width == 0)	/* x8 */
					dimm_config = 2;
				else
					die("Dual-rank x16 not supported\n");
			}
			s->dimm_config[chan] |= dimm_config << (i % DIMMS_PER_CHANNEL) * 2;
		}
		printk(BIOS_DEBUG, "  Config[CH%d] : %d\n", chan, s->dimm_config[chan]);
	}

}

static void checkreset_ddr2(int boot_path)
{
	u8 pmcon2;
	u32 pmsts;

	if (boot_path >= 1) {
		pmsts = mchbar_read32(PMSTS_MCHBAR);
		if (!(pmsts & 1))
			printk(BIOS_DEBUG, "Channel 0 possibly not in self refresh\n");
		if (!(pmsts & 2))
			printk(BIOS_DEBUG, "Channel 1 possibly not in self refresh\n");
	}

	pmcon2 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);

	if (pmcon2 & 0x80) {
		pmcon2 &= ~0x80;
		pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa2, pmcon2);

		/* do magic 0xf0 thing. */
		pci_and_config8(HOST_BRIDGE, 0xf0, ~(1 << 2));

		pci_or_config8(HOST_BRIDGE, 0xf0, (1 << 2));

		full_reset();
	}
	pmcon2 |= 0x80;
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa2, pmcon2);
}

/**
 * @param boot_path: 0 = normal, 1 = reset, 2 = resume from s3
 */
void sdram_initialize(int boot_path, const u8 *spd_map)
{
	struct sysinfo s, *ctrl_cached;
	u8 reg8;
	int fast_boot, cbmem_was_inited;
	size_t mrc_size;

	timestamp_add_now(TS_INITRAM_START);
	printk(BIOS_DEBUG, "Setting up RAM controller.\n");

	pci_write_config8(HOST_BRIDGE, 0xdf, 0xff);

	memset(&s, 0, sizeof(struct sysinfo));

	ctrl_cached = mrc_cache_current_mmap_leak(MRC_TRAINING_DATA,
						  MRC_CACHE_VERSION,
						  &mrc_size);

	if (!ctrl_cached || mrc_size < sizeof(s)) {
		if (boot_path == BOOT_PATH_RESUME) {
			/* Failed S3 resume, reset to come up cleanly */
			system_reset();
		} else if (boot_path == BOOT_PATH_WARM_RESET) {
			/* On warm reset some of dram calibrations fail
			   and therefore requiring valid cached settings */
			full_reset();
		}
	}

	/* verify MRC cache for fast boot */
	if (boot_path != BOOT_PATH_RESUME && ctrl_cached) {
		/* check SPD checksum to make sure the DIMMs haven't been replaced */
		fast_boot = verify_spds(spd_map, ctrl_cached) == CB_SUCCESS;
		if (!fast_boot) {
			printk(BIOS_DEBUG,
			       "SPD checksums don't match, dimm's have been replaced\n");
		} else {
			find_fsb_speed(&s);
			fast_boot = s.max_fsb == ctrl_cached->max_fsb;
			if (!fast_boot)
				printk(BIOS_DEBUG,
				       "CPU FSB does not match and has been replaced\n");
		}
	} else {
		fast_boot = boot_path == BOOT_PATH_RESUME;
	}

	if (fast_boot) {
		printk(BIOS_DEBUG, "Using cached raminit settings\n");
		memcpy(&s, ctrl_cached, sizeof(s));
		s.boot_path = boot_path;
		mchinfo_ddr2(&s);
		print_selected_timings(&s);
	} else {
		s.boot_path = boot_path;
		s.spd_map[0] = spd_map[0];
		s.spd_map[1] = spd_map[1];
		s.spd_map[2] = spd_map[2];
		s.spd_map[3] = spd_map[3];
		checkreset_ddr2(s.boot_path);

		/* Detect dimms per channel */
		reg8 = pci_read_config8(HOST_BRIDGE, 0xe9);
		printk(BIOS_DEBUG, "Dimms per channel: %d\n", (reg8 & 0x10) ? 1 : 2);

		mchinfo_ddr2(&s);

		find_fsb_speed(&s);
		decode_spd_select_timings(&s);
		print_selected_timings(&s);
		find_dimm_config(&s);
	}

	do_raminit(&s, fast_boot);

	pci_and_config8(PCI_DEV(0, 0x1f, 0), 0xa2, (u8)~0x80);

	pci_or_config8(HOST_BRIDGE, 0xf4, 1);

	timestamp_add_now(TS_INITRAM_END);

	printk(BIOS_DEBUG, "RAM initialization finished.\n");

	int s3resume = boot_path == BOOT_PATH_RESUME;

	cbmem_was_inited = !cbmem_recovery(s3resume);
	if (!fast_boot)
		mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION, &s, sizeof(s));

	if (s3resume && !cbmem_was_inited) {
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	}

	printk(BIOS_DEBUG, "Memory initialized\n");
}
