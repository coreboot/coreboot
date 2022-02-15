/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <device/smbus_host.h>
#include <spd.h>
#include <console/console.h>
#include <lib.h>
#include <delay.h>
#include <timestamp.h>
#include "gm45.h"
#include "chip.h"

static const gmch_gfx_t gmch_gfx_types[][5] = {
/*  MAX_667MHz    MAX_533MHz    MAX_400MHz    MAX_333MHz    MAX_800MHz    */
  { GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN  },
  { GMCH_GM47,    GMCH_GM45,    GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_GM49     },
  { GMCH_GE45,    GMCH_GE45,    GMCH_GE45,    GMCH_GE45,    GMCH_GE45     },
  { GMCH_UNKNOWN, GMCH_GL43,	GMCH_GL40,    GMCH_UNKNOWN, GMCH_UNKNOWN  },
  { GMCH_UNKNOWN, GMCH_GS45,    GMCH_GS40,    GMCH_UNKNOWN, GMCH_UNKNOWN  },
  { GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN  },
  { GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN, GMCH_UNKNOWN  },
  { GMCH_PM45,    GMCH_PM45,    GMCH_PM45,    GMCH_PM45,    GMCH_PM45     },
};

void get_gmch_info(sysinfo_t *sysinfo)
{
	sysinfo->stepping = pci_read_config8(PCI_DEV(0, 0, 0), PCI_CLASS_REVISION);
	if ((sysinfo->stepping > STEPPING_B3) &&
	    (sysinfo->stepping != STEPPING_CONVERSION_A1))
		die("Unknown stepping.\n");
	if (sysinfo->stepping <= STEPPING_B3)
		printk(BIOS_DEBUG, "Stepping %c%d\n", 'A' + sysinfo->stepping / 4, sysinfo->stepping % 4);
	else
		printk(BIOS_DEBUG, "Conversion stepping A1\n");

	const u32 eax = cpuid_ext(0x04, 0).eax;
	sysinfo->cores = ((eax >> 26) & 0x3f) + 1;
	printk(BIOS_SPEW, "%d CPU cores\n", sysinfo->cores);

	u32 capid = pci_read_config16(PCI_DEV(0, 0, 0), D0F0_CAPID0+8);
	if (!(capid & (1<<(79-64)))) {
		printk(BIOS_SPEW, "iTPM enabled\n");
	}

	capid = pci_read_config32(PCI_DEV(0, 0, 0), D0F0_CAPID0+4);
	if (!(capid & (1<<(57-32)))) {
		printk(BIOS_SPEW, "ME enabled\n");
	}

	if (!(capid & (1<<(56-32)))) {
		printk(BIOS_SPEW, "AMT enabled\n");
	}

	sysinfo->max_ddr2_mhz = (capid & (1<<(53-32)))?667:800;
	printk(BIOS_SPEW, "capable of DDR2 of %d MHz or lower\n", sysinfo->max_ddr2_mhz);

	if (!(capid & (1<<(48-32)))) {
		printk(BIOS_SPEW, "VT-d enabled\n");
	}

	const u32 gfx_variant = (capid>>(42-32)) & 0x7;
	const u32 render_freq = ((capid>>(50-32) & 0x1) << 2) | ((capid>>(35-32)) & 0x3);
	if (render_freq <= 4)
		sysinfo->gfx_type = gmch_gfx_types[gfx_variant][render_freq];
	else
		sysinfo->gfx_type = GMCH_UNKNOWN;
	switch (sysinfo->gfx_type) {
		case GMCH_GM45:
			printk(BIOS_SPEW, "GMCH: GM45\n");
			break;
		case GMCH_GM47:
			printk(BIOS_SPEW, "GMCH: GM47\n");
			break;
		case GMCH_GM49:
			printk(BIOS_SPEW, "GMCH: GM49\n");
			break;
		case GMCH_GE45:
			printk(BIOS_SPEW, "GMCH: GE45\n");
			break;
		case GMCH_GL40:
			printk(BIOS_SPEW, "GMCH: GL40\n");
			break;
		case GMCH_GL43:
			printk(BIOS_SPEW, "GMCH: GL43\n");
			break;
		case GMCH_GS40:
			printk(BIOS_SPEW, "GMCH: GS40\n");
			break;
		case GMCH_GS45:
			printk(BIOS_SPEW, "GMCH: GS45, using %s-power mode\n",
			       sysinfo->gs45_low_power_mode ? "low" : "high");
			break;
		case GMCH_PM45:
			printk(BIOS_SPEW, "GMCH: PM45\n");
			break;
		case GMCH_UNKNOWN:
			printk(BIOS_SPEW, "unknown GMCH\n");
			break;
	}

	sysinfo->txt_enabled = !(capid & (1 << (37-32)));
	if (sysinfo->txt_enabled) {
		printk(BIOS_SPEW, "TXT enabled\n");
	}

	switch (render_freq) {
		case 4:
			sysinfo->max_render_mhz = 800;
			break;
		case 0:
			sysinfo->max_render_mhz = 667;
			break;
		case 1:
			sysinfo->max_render_mhz = 533;
			break;
		case 2:
			sysinfo->max_render_mhz = 400;
			break;
		case 3:
			sysinfo->max_render_mhz = 333;
			break;
		default:
			printk(BIOS_SPEW, "Unknown render frequency\n");
			sysinfo->max_render_mhz = 0;
			break;
	}
	if (sysinfo->max_render_mhz != 0) {
		printk(BIOS_SPEW, "Render frequency: %d MHz\n", sysinfo->max_render_mhz);
	}

	if (!(capid & (1<<(33-32)))) {
		printk(BIOS_SPEW, "IGD enabled\n");
	}

	if (!(capid & (1<<(32-32)))) {
		printk(BIOS_SPEW, "PCIe-to-GMCH enabled\n");
	}

	capid = pci_read_config32(PCI_DEV(0, 0, 0), D0F0_CAPID0);

	u32 ddr_cap = capid>>30 & 0x3;
	switch (ddr_cap) {
		case 0:
			sysinfo->max_ddr3_mt = 1067;
			break;
		case 1:
			sysinfo->max_ddr3_mt = 800;
			break;
		case 2:
		case 3:
			printk(BIOS_SPEW, "GMCH not DDR3 capable\n");
			sysinfo->max_ddr3_mt = 0;
			break;
	}
	if (sysinfo->max_ddr3_mt != 0) {
		printk(BIOS_SPEW, "GMCH supports DDR3 with %d MT or less\n", sysinfo->max_ddr3_mt);
	}

	const unsigned int max_fsb = (capid >> 28) & 0x3;
	switch (max_fsb) {
		case 1:
			sysinfo->max_fsb_mhz = 1067;
			break;
		case 2:
			sysinfo->max_fsb_mhz = 800;
			break;
		case 3:
			sysinfo->max_fsb_mhz = 667;
			break;
		default:
			die("unknown FSB capability\n");
			break;
	}
	if (sysinfo->max_fsb_mhz != 0) {
		printk(BIOS_SPEW, "GMCH supports FSB with up to %d MHz\n", sysinfo->max_fsb_mhz);
	}
	sysinfo->max_fsb = max_fsb - 1;
}

/*
 * Detect if the system went through an interrupted RAM init or is incon-
 * sistent. If so, initiate a cold reboot. Otherwise mark the system to be
 * in RAM init, so this function would detect it on an erroneous reboot.
 */
void enter_raminit_or_reset(void)
{
	/* Interrupted RAM init or inconsistent system? */
	u8 reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);

	if (reg8 & (1 << 2)) { /* S4-assertion-width violation */
		/* Ignore S4-assertion-width violation like original BIOS. */
		printk(BIOS_WARNING, "Ignoring S4-assertion-width violation.\n");
		/* Bit2 is R/WC, so it will clear itself below. */
	}

	if (reg8 & (1 << 7)) { /* interrupted RAM init */
		/* Don't enable S4-assertion stretch. Makes trouble on roda/rk9.
		reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
		pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, reg8 | 0x08);
		*/

		/* Clear bit7. */
		pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa2, reg8 & ~(1 << 7));

		printk(BIOS_INFO, "Interrupted RAM init, reset required.\n");
		gm45_early_reset();
	}
	/* Mark system to be in RAM init. */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa2, reg8 | (1 << 7));
}

/* For a detected DIMM, test the value of an SPD byte to
   match the expected value after masking some bits. */
static int test_dimm(sysinfo_t *const sysinfo,
		     int dimm, int addr, int bitmask, int expected)
{
	return (smbus_read_byte(sysinfo->spd_map[dimm], addr) & bitmask) == expected;
}

/* This function dies if dimm is unsuitable for the chipset. */
static void verify_ddr3_dimm(sysinfo_t *const sysinfo, int dimm)
{
	if (!test_dimm(sysinfo, dimm, 3, 15, 3))
		die("Chipset only supports SO-DIMM\n");

	if (!test_dimm(sysinfo, dimm, 8, 0x18, 0))
		die("Chipset doesn't support ECC RAM\n");

	if (!test_dimm(sysinfo, dimm, 7, 0x38, 0) &&
			!test_dimm(sysinfo, dimm, 7, 0x38, 8))
		die("Chipset wants single or double sided DIMMs\n");

	if (!test_dimm(sysinfo, dimm, 7, 7, 1) &&
			!test_dimm(sysinfo, dimm, 7, 7, 2))
		die("Chipset requires x8 or x16 width\n");

	if (!test_dimm(sysinfo, dimm, 4, 0x0f, 0) &&
			!test_dimm(sysinfo, dimm, 4, 0x0f, 1) &&
			!test_dimm(sysinfo, dimm, 4, 0x0f, 2) &&
			!test_dimm(sysinfo, dimm, 4, 0x0f, 3))
		die("Chipset requires 256Mb, 512Mb, 1Gb or 2Gb chips.");

	if (!test_dimm(sysinfo, dimm, 4, 0x70, 0))
		die("Chipset requires 8 banks on DDR3\n");

	/* How to check if burst length is 8?
	   Other values are not supported, are they even possible? */

	if (!test_dimm(sysinfo, dimm, 10, 0xff, 1))
		die("Code assumes 1/8ns MTB\n");

	if (!test_dimm(sysinfo, dimm, 11, 0xff, 8))
		die("Code assumes 1/8ns MTB\n");

	if (!test_dimm(sysinfo, dimm, 62, 0x9f, 0) &&
			!test_dimm(sysinfo, dimm, 62, 0x9f, 1) &&
			!test_dimm(sysinfo, dimm, 62, 0x9f, 2) &&
			!test_dimm(sysinfo, dimm, 62, 0x9f, 3) &&
			!test_dimm(sysinfo, dimm, 62, 0x9f, 5))
		die("Only raw card types A, B, C, D and F are supported.\n");
}

/* For every detected DIMM, test if it's suitable for the chipset. */
static void verify_ddr3(sysinfo_t *const sysinfo, int mask)
{
	int cur = 0;
	while (mask) {
		if (mask & 1) {
			verify_ddr3_dimm(sysinfo, cur);
		}
		mask >>= 1;
		cur++;
	}
}

typedef struct {
	int dimm_mask;
	struct {
		unsigned int rows;
		unsigned int cols;
		unsigned int chip_capacity;
		unsigned int banks;
		unsigned int ranks;
		unsigned int cas_latencies;
		unsigned int tAAmin;
		unsigned int tCKmin;
		unsigned int width;
		unsigned int tRAS;
		unsigned int tRP;
		unsigned int tRCD;
		unsigned int tWR;
		unsigned int page_size;
		unsigned int raw_card;
	} channel[2];
} spdinfo_t;
/*
 * This function collects RAM characteristics from SPD, assuming that RAM
 * is generally within chipset's requirements, since verify_ddr3() passed.
 */
static void collect_ddr3(sysinfo_t *const sysinfo, spdinfo_t *const config)
{
	int mask = config->dimm_mask;
	int cur = 0;
	while (mask != 0) {
		/* FIXME: support several dimms on same channel.  */
		if ((mask & 1) && sysinfo->spd_map[2 * cur]) {
			int tmp;
			const int smb_addr = sysinfo->spd_map[2 * cur];

			config->channel[cur].rows = ((smbus_read_byte(smb_addr, 5) >> 3) & 7) + 12;
			config->channel[cur].cols = (smbus_read_byte(smb_addr, 5) & 7) + 9;

			config->channel[cur].chip_capacity = smbus_read_byte(smb_addr, 4) & 0xf;

			config->channel[cur].banks = 8; /* GM45 only accepts this for DDR3.
							   verify_ddr3() fails for other values. */
			config->channel[cur].ranks = ((smbus_read_byte(smb_addr, 7) >> 3) & 7) + 1;

			config->channel[cur].cas_latencies =
				((smbus_read_byte(smb_addr, 15) << 8) | smbus_read_byte(smb_addr, 14))
				<< 4; /* so bit x is CAS x */
			config->channel[cur].tAAmin = smbus_read_byte(smb_addr, 16); /* in MTB */
			config->channel[cur].tCKmin = smbus_read_byte(smb_addr, 12); /* in MTB */

			config->channel[cur].width = smbus_read_byte(smb_addr, 7) & 7;
			config->channel[cur].page_size = config->channel[cur].width *
								(1 << config->channel[cur].cols); /* in Bytes */

			tmp = smbus_read_byte(smb_addr, 21);
			config->channel[cur].tRAS = smbus_read_byte(smb_addr, 22) | ((tmp & 0xf) << 8);
			config->channel[cur].tRP = smbus_read_byte(smb_addr, 20);
			config->channel[cur].tRCD = smbus_read_byte(smb_addr, 18);
			config->channel[cur].tWR = smbus_read_byte(smb_addr, 17);

			config->channel[cur].raw_card = smbus_read_byte(smb_addr, 62) & 0x1f;
		}
		cur++;
		mask >>= 2;
	}
}

static fsb_clock_t read_fsb_clock(void)
{
	switch (mchbar_read32(CLKCFG_MCHBAR) & CLKCFG_FSBCLK_MASK) {
	case 6:
		return FSB_CLOCK_1067MHz;
	case 2:
		return FSB_CLOCK_800MHz;
	case 3:
		return FSB_CLOCK_667MHz;
	default:
		die("Unsupported FSB clock.\n");
	}
}
static mem_clock_t clock_index(const unsigned int clock)
{
	switch (clock) {
		case 533:	return MEM_CLOCK_533MHz;
		case 400:	return MEM_CLOCK_400MHz;
		case 333:	return MEM_CLOCK_333MHz;
		default:	die("Unknown clock value.\n");
	}
	return -1; /* Won't be reached. */
}
static void normalize_clock(unsigned int *const clock)
{
	if (*clock >= 533)
		*clock = 533;
	else if (*clock >= 400)
		*clock = 400;
	else if (*clock >= 333)
		*clock = 333;
	else
		*clock = 0;
}
static void lower_clock(unsigned int *const clock)
{
	--*clock;
	normalize_clock(clock);
}
static unsigned int find_common_clock_cas(sysinfo_t *const sysinfo,
					  const spdinfo_t *const spdinfo)
{
	/* various constraints must be fulfilled:
	   CAS * tCK < 20ns == 160MTB
	   tCK_max >= tCK >= tCK_min
	   CAS >= roundup(tAA_min/tCK)
	   CAS supported
	   Clock(MHz) = 1000 / tCK(ns)
	   Clock(MHz) = 8000 / tCK(MTB)
	   AND BTW: Clock(MT) = 2000 / tCK(ns) - intel uses MTs but calls them MHz
	 */
	int i;

	/* Calculate common cas_latencies mask, tCKmin and tAAmin. */
	unsigned int cas_latencies = (unsigned int)-1;
	unsigned int tCKmin = 0, tAAmin = 0;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, i) {
		cas_latencies &= spdinfo->channel[i].cas_latencies;
		if (spdinfo->channel[i].tCKmin > tCKmin)
			tCKmin = spdinfo->channel[i].tCKmin;
		if (spdinfo->channel[i].tAAmin > tAAmin)
			tAAmin = spdinfo->channel[i].tAAmin;
	}

	/* Get actual value of fsb clock. */
	sysinfo->selected_timings.fsb_clock = read_fsb_clock();
	unsigned int fsb_mhz = 0;
	switch (sysinfo->selected_timings.fsb_clock) {
		case FSB_CLOCK_1067MHz:	fsb_mhz = 1067; break;
		case FSB_CLOCK_800MHz:	fsb_mhz =  800; break;
		case FSB_CLOCK_667MHz:	fsb_mhz =  667; break;
	}

	unsigned int clock = 8000 / tCKmin;
	if ((clock > sysinfo->max_ddr3_mt / 2) || (clock > fsb_mhz / 2)) {
		int new_clock = MIN(sysinfo->max_ddr3_mt / 2, fsb_mhz / 2);
		printk(BIOS_SPEW, "DIMMs support %d MHz, but chipset only runs at up to %d. Limiting...\n",
			clock, new_clock);
		clock = new_clock;
	}
	normalize_clock(&clock);

	/* Find compatible clock / CAS pair. */
	unsigned int tCKproposed;
	unsigned int CAS;
	while (1) {
		if (!clock)
			die("Couldn't find compatible clock / CAS settings.\n");
		tCKproposed = 8000 / clock;
		CAS = DIV_ROUND_UP(tAAmin, tCKproposed);
		printk(BIOS_SPEW, "Trying CAS %u, tCK %u.\n", CAS, tCKproposed);
		for (; CAS <= DDR3_MAX_CAS; ++CAS)
			if (cas_latencies & (1 << CAS))
				break;
		if ((CAS <= DDR3_MAX_CAS) && (CAS * tCKproposed < 160)) {
			/* Found good CAS. */
			printk(BIOS_SPEW, "Found compatible clock / CAS pair: %u / %u.\n", clock, CAS);
			break;
		}
		lower_clock(&clock);
	}
	sysinfo->selected_timings.CAS = CAS;
	sysinfo->selected_timings.mem_clock = clock_index(clock);

	return tCKproposed;
}

static void calculate_derived_timings(sysinfo_t *const sysinfo,
				      const unsigned int tCLK,
				      const spdinfo_t *const spdinfo)
{
	int i;

	/* Calculate common tRASmin, tRPmin, tRCDmin and tWRmin. */
	unsigned int tRASmin = 0, tRPmin = 0, tRCDmin = 0, tWRmin = 0;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, i) {
		if (spdinfo->channel[i].tRAS > tRASmin)
			tRASmin = spdinfo->channel[i].tRAS;
		if (spdinfo->channel[i].tRP > tRPmin)
			tRPmin = spdinfo->channel[i].tRP;
		if (spdinfo->channel[i].tRCD > tRCDmin)
			tRCDmin = spdinfo->channel[i].tRCD;
		if (spdinfo->channel[i].tWR > tWRmin)
			tWRmin = spdinfo->channel[i].tWR;
	}
	tRASmin = DIV_ROUND_UP(tRASmin, tCLK);
	tRPmin = DIV_ROUND_UP(tRPmin, tCLK);
	tRCDmin = DIV_ROUND_UP(tRCDmin, tCLK);
	tWRmin = DIV_ROUND_UP(tWRmin, tCLK);

	/* Lookup tRFC and calculate common tRFCmin. */
	const unsigned int tRFC_from_clock_and_cap[][4] = {
	/*             CAP_256M	   CAP_512M      CAP_1G      CAP_2G */
	/* 533MHz */ {       40,         56,         68,        104 },
	/* 400MHz */ {       30,         42,         51,         78 },
	/* 333MHz */ {       25,         35,         43,         65 },
	};
	unsigned int tRFCmin = 0;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, i) {
		const unsigned int tRFC = tRFC_from_clock_and_cap
			[sysinfo->selected_timings.mem_clock][spdinfo->channel[i].chip_capacity];
		if (tRFC > tRFCmin)
			tRFCmin = tRFC;
	}

	/* Calculate common tRD from CAS and FSB and DRAM clocks. */
	unsigned int tRDmin = sysinfo->selected_timings.CAS;
	switch (sysinfo->selected_timings.fsb_clock) {
	case FSB_CLOCK_667MHz:
		tRDmin += 1;
		break;
	case FSB_CLOCK_800MHz:
		tRDmin += 2;
		break;
	case FSB_CLOCK_1067MHz:
		tRDmin += 3;
		if (sysinfo->selected_timings.mem_clock == MEM_CLOCK_1067MT)
			tRDmin += 1;
		break;
	}

	/* Calculate common tRRDmin. */
	unsigned int tRRDmin = 0;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, i) {
		unsigned int tRRD = 2 + (spdinfo->channel[i].page_size / 1024);
		if (sysinfo->selected_timings.mem_clock == MEM_CLOCK_1067MT)
			tRRD += (spdinfo->channel[i].page_size / 1024);
		if (tRRD > tRRDmin)
			tRRDmin = tRRD;
	}

	/* Lookup and calculate common tFAWmin. */
	unsigned int tFAW_from_pagesize_and_clock[][3] = {
	/*		533MHz	400MHz	333MHz */
	/* 1K */ {	20,	15,	13	},
	/* 2K */ {	27,	20,	17	},
	};
	unsigned int tFAWmin = 0;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, i) {
		const unsigned int tFAW = tFAW_from_pagesize_and_clock
			[spdinfo->channel[i].page_size / 1024 - 1]
			[sysinfo->selected_timings.mem_clock];
		if (tFAW > tFAWmin)
			tFAWmin = tFAW;
	}

	/* Refresh rate is fixed. */
	unsigned int tWL;
	if (sysinfo->selected_timings.mem_clock == MEM_CLOCK_1067MT) {
		tWL = 6;
	} else {
		tWL = 5;
	}

	printk(BIOS_SPEW, "Timing values:\n"
		" tCLK:  %3u\n"
		" tRAS:  %3u\n"
		" tRP:   %3u\n"
		" tRCD:  %3u\n"
		" tRFC:  %3u\n"
		" tWR:   %3u\n"
		" tRD:   %3u\n"
		" tRRD:  %3u\n"
		" tFAW:  %3u\n"
		" tWL:   %3u\n",
		tCLK, tRASmin, tRPmin, tRCDmin, tRFCmin, tWRmin, tRDmin, tRRDmin, tFAWmin, tWL);

	sysinfo->selected_timings.tRAS	= tRASmin;
	sysinfo->selected_timings.tRP	= tRPmin;
	sysinfo->selected_timings.tRCD	= tRCDmin;
	sysinfo->selected_timings.tRFC	= tRFCmin;
	sysinfo->selected_timings.tWR	= tWRmin;
	sysinfo->selected_timings.tRD	= tRDmin;
	sysinfo->selected_timings.tRRD	= tRRDmin;
	sysinfo->selected_timings.tFAW	= tFAWmin;
	sysinfo->selected_timings.tWL	= tWL;
}

static void collect_dimm_config(sysinfo_t *const sysinfo)
{
	int i;
	spdinfo_t spdinfo;

	spdinfo.dimm_mask = 0;
	sysinfo->spd_type = 0;

	for (i = 0; i < 4; i++)
		if (sysinfo->spd_map[i]) {
			const u8 spd = smbus_read_byte(sysinfo->spd_map[i], 2);
			printk (BIOS_DEBUG, "%x:%x:%x\n",
				i, sysinfo->spd_map[i],
				spd);
			if ((spd == 7) || (spd == 8) || (spd == 0xb)) {
				spdinfo.dimm_mask |= 1 << i;
				if (sysinfo->spd_type && sysinfo->spd_type != spd) {
					die("Multiple types of DIMM installed in the system, don't do that!\n");
				}
				sysinfo->spd_type = spd;
			}
		}
	if (spdinfo.dimm_mask == 0) {
		die("Could not find any DIMM.\n");
	}

	/* Normalize spd_type to 1, 2, 3. */
	sysinfo->spd_type = (sysinfo->spd_type & 1) | ((sysinfo->spd_type & 8) >> 2);
	printk(BIOS_SPEW, "DDR mask %x, DDR %d\n", spdinfo.dimm_mask, sysinfo->spd_type);

	if (sysinfo->spd_type == DDR2) {
		die("DDR2 not supported at this time.\n");
	} else if (sysinfo->spd_type == DDR3) {
		verify_ddr3(sysinfo, spdinfo.dimm_mask);
		collect_ddr3(sysinfo, &spdinfo);
	} else {
		die("Will never support DDR1.\n");
	}

	for (i = 0; i < 2; i++) {
		if ((spdinfo.dimm_mask >> (i*2)) & 1) {
			printk(BIOS_SPEW, "Bank %d populated:\n"
					  " Raw card type: %4c\n"
					  " Row addr bits: %4u\n"
					  " Col addr bits: %4u\n"
					  " byte width:    %4u\n"
					  " page size:     %4u\n"
					  " banks:         %4u\n"
					  " ranks:         %4u\n"
					  " tAAmin:    %3u\n"
					  " tCKmin:    %3u\n"
					  "  Max clock: %3u MHz\n"
					  " CAS:       0x%04x\n",
				i, spdinfo.channel[i].raw_card + 'A',
				spdinfo.channel[i].rows, spdinfo.channel[i].cols,
				spdinfo.channel[i].width, spdinfo.channel[i].page_size,
				spdinfo.channel[i].banks, spdinfo.channel[i].ranks,
				spdinfo.channel[i].tAAmin, spdinfo.channel[i].tCKmin,
				8000 / spdinfo.channel[i].tCKmin, spdinfo.channel[i].cas_latencies);
		}
	}

	FOR_EACH_CHANNEL(i) {
		sysinfo->dimms[i].card_type =
			(spdinfo.dimm_mask & (1 << (i * 2))) ? spdinfo.channel[i].raw_card + 0xa : 0;
	}

	/* Find common memory clock and CAS. */
	const unsigned int tCLK = find_common_clock_cas(sysinfo, &spdinfo);

	/* Calculate other timings from clock and CAS. */
	calculate_derived_timings(sysinfo, tCLK, &spdinfo);

	/* Initialize DIMM infos. */
	/* Always prefer interleaved over async channel mode. */
	FOR_EACH_CHANNEL(i) {
		IF_CHANNEL_POPULATED(sysinfo->dimms, i) {
			sysinfo->dimms[i].banks = spdinfo.channel[i].banks;
			sysinfo->dimms[i].ranks = spdinfo.channel[i].ranks;

			/* .width is 1 for x8 or 2 for x16, bus width is 8 bytes. */
			const unsigned int chips_per_rank = 8 / spdinfo.channel[i].width;

			sysinfo->dimms[i].chip_width = spdinfo.channel[i].width;
			sysinfo->dimms[i].chip_capacity = spdinfo.channel[i].chip_capacity;
			sysinfo->dimms[i].page_size = spdinfo.channel[i].page_size * chips_per_rank;
			sysinfo->dimms[i].rank_capacity_mb =
				/* offset of chip_capacity is 8 (256M), therefore, add 8
				   chip_capacity is in Mbit, we want MByte, therefore, subtract 3 */
				(1 << (spdinfo.channel[i].chip_capacity + 8 - 3)) * chips_per_rank;
		}
	}
	if (CHANNEL_IS_POPULATED(sysinfo->dimms, 0) &&
			CHANNEL_IS_POPULATED(sysinfo->dimms, 1))
		sysinfo->selected_timings.channel_mode = CHANNEL_MODE_DUAL_INTERLEAVED;
	else
		sysinfo->selected_timings.channel_mode = CHANNEL_MODE_SINGLE;
}

static void reset_on_bad_warmboot(void)
{
	/* Check self refresh channel status. */
	const u32 reg = mchbar_read32(PMSTS_MCHBAR);
	/* Clear status bits. R/WC */
	mchbar_write32(PMSTS_MCHBAR, reg);
	if ((reg & PMSTS_WARM_RESET) && !(reg & PMSTS_BOTH_SELFREFRESH)) {
		printk(BIOS_INFO, "DRAM was not in self refresh "
			"during warm boot, reset required.\n");
		gm45_early_reset();
	}
}

static void set_system_memory_frequency(const timings_t *const timings)
{
	mchbar_clrbits16(CLKCFG_MCHBAR + 0x60, 1 << 15);
	mchbar_clrbits16(CLKCFG_MCHBAR + 0x48, 1 << 15);

	/* Calculate wanted frequency setting. */
	const int want_freq = 6 - timings->mem_clock;

	/* Read current memory frequency. */
	const u32 clkcfg = mchbar_read32(CLKCFG_MCHBAR);
	int cur_freq = (clkcfg & CLKCFG_MEMCLK_MASK) >> CLKCFG_MEMCLK_SHIFT;
	if (0 == cur_freq) {
		/* Try memory frequency from scratchpad. */
		printk(BIOS_DEBUG, "Reading current memory frequency from scratchpad.\n");
		cur_freq = (mchbar_read16(SSKPD_MCHBAR) & SSKPD_CLK_MASK) >> SSKPD_CLK_SHIFT;
	}

	if (cur_freq != want_freq) {
		printk(BIOS_DEBUG, "Changing memory frequency: old %x, new %x.\n", cur_freq, want_freq);
		/* When writing new frequency setting, reset, then set update bit. */
		mchbar_clrsetbits32(CLKCFG_MCHBAR, CLKCFG_UPDATE | CLKCFG_MEMCLK_MASK,
					  want_freq << CLKCFG_MEMCLK_SHIFT);
		mchbar_clrsetbits32(CLKCFG_MCHBAR, CLKCFG_MEMCLK_MASK,
					  want_freq << CLKCFG_MEMCLK_SHIFT | CLKCFG_UPDATE);
		/* Reset update bit. */
		mchbar_clrbits32(CLKCFG_MCHBAR, CLKCFG_UPDATE);
	}

	if ((timings->fsb_clock == FSB_CLOCK_1067MHz) && (timings->mem_clock == MEM_CLOCK_667MT)) {
		mchbar_write32(CLKCFG_MCHBAR + 0x16, 0x000030f0);
		mchbar_write32(CLKCFG_MCHBAR + 0x64, 0x000050c1);

		mchbar_clrsetbits32(CLKCFG_MCHBAR, 1 << 12, 1 << 17);
		mchbar_setbits32(CLKCFG_MCHBAR, 1 << 17 | 1 << 12);
		mchbar_clrbits32(CLKCFG_MCHBAR, 1 << 12);

		mchbar_write32(CLKCFG_MCHBAR + 0x04, 0x9bad1f1f);
		mchbar_write8(CLKCFG_MCHBAR + 0x08, 0xf4);
		mchbar_write8(CLKCFG_MCHBAR + 0x0a, 0x43);
		mchbar_write8(CLKCFG_MCHBAR + 0x0c, 0x10);
		mchbar_write8(CLKCFG_MCHBAR + 0x0d, 0x80);
		mchbar_write32(CLKCFG_MCHBAR + 0x50, 0x0b0e151b);
		mchbar_write8(CLKCFG_MCHBAR + 0x54, 0xb4);
		mchbar_write8(CLKCFG_MCHBAR + 0x55, 0x10);
		mchbar_write8(CLKCFG_MCHBAR + 0x56, 0x08);

		mchbar_setbits32(CLKCFG_MCHBAR, 1 << 10);
		mchbar_setbits32(CLKCFG_MCHBAR, 1 << 11);
		mchbar_clrbits32(CLKCFG_MCHBAR, 1 << 10);
		mchbar_clrbits32(CLKCFG_MCHBAR, 1 << 11);
	}

	mchbar_setbits32(CLKCFG_MCHBAR + 0x48, 0x3f << 24);
}

int raminit_read_vco_index(void)
{
	switch (mchbar_read8(HPLLVCO_MCHBAR) & 0x7) {
	case VCO_2666:
		return 0;
	case VCO_3200:
		return 1;
	case VCO_4000:
		return 2;
	case VCO_5333:
		return 3;
	default:
		die("Unknown VCO frequency.\n");
		return 0;
	}
}
static void set_igd_memory_frequencies(const sysinfo_t *const sysinfo)
{
	const int gfx_idx = ((sysinfo->gfx_type == GMCH_GS45) &&
				!sysinfo->gs45_low_power_mode)
		? (GMCH_GS45 + 1) : sysinfo->gfx_type;

	/* Render and sampler frequency values seem to be some kind of factor. */
	const u16 render_freq_from_vco_and_gfxtype[][10] = {
	/*              GM45  GM47  GM49  GE45  GL40  GL43  GS40  GS45 (perf) */
	/* VCO 2666 */ { 0xd,  0xd,  0xe,  0xd,  0xb,  0xd,  0xb,  0xa,  0xd },
	/* VCO 3200 */ { 0xd,  0xe,  0xf,  0xd,  0xb,  0xd,  0xb,  0x9,  0xd },
	/* VCO 4000 */ { 0xc,  0xd,  0xf,  0xc,  0xa,  0xc,  0xa,  0x9,  0xc },
	/* VCO 5333 */ { 0xb,  0xc,  0xe,  0xb,  0x9,  0xb,  0x9,  0x8,  0xb },
	};
	const u16 sampler_freq_from_vco_and_gfxtype[][10] = {
	/*              GM45  GM47  GM49  GE45  GL40  GL43  GS40  GS45 (perf) */
	/* VCO 2666 */ { 0xc,  0xc,  0xd,  0xc,  0x9,  0xc,  0x9,  0x8,  0xc },
	/* VCO 3200 */ { 0xc,  0xd,  0xe,  0xc,  0x9,  0xc,  0x9,  0x8,  0xc },
	/* VCO 4000 */ { 0xa,  0xc,  0xd,  0xa,  0x8,  0xa,  0x8,  0x8,  0xa },
	/* VCO 5333 */ { 0xa,  0xa,  0xc,  0xa,  0x7,  0xa,  0x7,  0x6,  0xa },
	};
	const u16 display_clock_select_from_gfxtype[] = {
		/* GM45  GM47  GM49  GE45  GL40  GL43  GS40  GS45 (perf) */
		      1,    1,    1,    1,    1,    1,    1,    0,    1
	};

	if (pci_read_config16(GCFGC_PCIDEV, 0) != 0x8086) {
		printk(BIOS_DEBUG, "Skipping IGD memory frequency setting.\n");
		return;
	}

	mchbar_write16(0x119e, 0xa800);
	mchbar_clrsetbits16(0x11c0, 0xff << 8, 0x01 << 8);
	mchbar_write16(0x119e, 0xb800);
	mchbar_setbits8(0x0f10, 1 << 7);

	/* Read VCO. */
	const int vco_idx = raminit_read_vco_index();
	printk(BIOS_DEBUG, "Setting IGD memory frequencies for VCO #%d.\n", vco_idx);

	const u32 freqcfg =
		((render_freq_from_vco_and_gfxtype[vco_idx][gfx_idx]
			<< GCFGC_CR_SHIFT) & GCFGC_CR_MASK) |
		((sampler_freq_from_vco_and_gfxtype[vco_idx][gfx_idx]
			<< GCFGC_CS_SHIFT) & GCFGC_CS_MASK);

	/* Set frequencies, clear update bit. */
	u32 gcfgc = pci_read_config16(GCFGC_PCIDEV, GCFGC_OFFSET);
	gcfgc &= ~(GCFGC_CS_MASK | GCFGC_UPDATE | GCFGC_CR_MASK);
	gcfgc |= freqcfg;
	pci_write_config16(GCFGC_PCIDEV, GCFGC_OFFSET, gcfgc);

	/* Set frequencies, set update bit. */
	gcfgc = pci_read_config16(GCFGC_PCIDEV, GCFGC_OFFSET);
	gcfgc &= ~(GCFGC_CS_MASK | GCFGC_CR_MASK);
	gcfgc |= freqcfg | GCFGC_UPDATE;
	pci_write_config16(GCFGC_PCIDEV, GCFGC_OFFSET, gcfgc);

	/* Clear update bit. */
	pci_and_config16(GCFGC_PCIDEV, GCFGC_OFFSET, ~GCFGC_UPDATE);

	/* Set display clock select bit. */
	pci_write_config16(GCFGC_PCIDEV, GCFGC_OFFSET,
		(pci_read_config16(GCFGC_PCIDEV, GCFGC_OFFSET) & ~GCFGC_CD_MASK) |
		(display_clock_select_from_gfxtype[gfx_idx] << GCFGC_CD_SHIFT));
}

static void configure_dram_control_mode(const timings_t *const timings, const dimminfo_t *const dimms)
{
	int ch, r;

	FOR_EACH_CHANNEL(ch) {
		unsigned int mchbar = CxDRC0_MCHBAR(ch);
		u32 cxdrc = mchbar_read32(mchbar);
		cxdrc &= ~CxDRC0_RANKEN_MASK;
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r)
			cxdrc |= CxDRC0_RANKEN(r);
		cxdrc = (cxdrc & ~CxDRC0_RMS_MASK) |
				/* Always 7.8us for DDR3: */
				CxDRC0_RMS_78US;
		mchbar_write32(mchbar, cxdrc);

		mchbar = CxDRC1_MCHBAR(ch);
		cxdrc = mchbar_read32(mchbar);
		cxdrc |= CxDRC1_NOTPOP_MASK;
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r)
			cxdrc &= ~CxDRC1_NOTPOP(r);
		cxdrc |= CxDRC1_MUSTWR;
		mchbar_write32(mchbar, cxdrc);

		mchbar = CxDRC2_MCHBAR(ch);
		cxdrc = mchbar_read32(mchbar);
		cxdrc |= CxDRC2_NOTPOP_MASK;
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r)
			cxdrc &= ~CxDRC2_NOTPOP(r);
		cxdrc |= CxDRC2_MUSTWR;
		if (timings->mem_clock == MEM_CLOCK_1067MT)
			cxdrc |= CxDRC2_CLK1067MT;
		mchbar_write32(mchbar, cxdrc);
	}
}

static void rcomp_initialization(const stepping_t stepping, const int sff)
{
	/* Program RCOMP codes. */
	if (sff)
		die("SFF platform unsupported in RCOMP initialization.\n");
	/* Values are for DDR3. */
	mchbar_clrbits8(0x6ac, 0x0f);
	mchbar_write8(0x6b0,   0x55);
	mchbar_clrbits8(0x6ec, 0x0f);
	mchbar_write8(0x6f0,   0x66);
	mchbar_clrbits8(0x72c, 0x0f);
	mchbar_write8(0x730,   0x66);
	mchbar_clrbits8(0x76c, 0x0f);
	mchbar_write8(0x770,   0x66);
	mchbar_clrbits8(0x7ac, 0x0f);
	mchbar_write8(0x7b0,   0x66);
	mchbar_clrbits8(0x7ec, 0x0f);
	mchbar_write8(0x7f0,   0x66);
	mchbar_clrbits8(0x86c, 0x0f);
	mchbar_write8(0x870,   0x55);
	mchbar_clrbits8(0x8ac, 0x0f);
	mchbar_write8(0x8b0,   0x66);
	/* ODT multiplier bits. */
	mchbar_clrsetbits32(0x04d0, 7 << 3 | 7 << 0, 2 << 3 | 2 << 0);

	/* Perform RCOMP calibration for DDR3. */
	raminit_rcomp_calibration(stepping);

	/* Run initial RCOMP. */
	mchbar_setbits32(0x418, 1 << 17);
	mchbar_clrbits32(0x40c, 1 << 23);
	mchbar_clrbits32(0x41c, 1 << 7 | 1 << 3);
	mchbar_setbits32(0x400, 1);
	while (mchbar_read32(0x400) & 1) {}

	/* Run second RCOMP. */
	mchbar_setbits32(0x40c, 1 << 19);
	mchbar_setbits32(0x400, 1);
	while (mchbar_read32(0x400) & 1) {}

	/* Cleanup and start periodic RCOMP. */
	mchbar_clrbits32(0x40c, 1 << 19);
	mchbar_setbits32(0x40c, 1 << 23);
	mchbar_clrbits32(0x418, 1 << 17);
	mchbar_setbits32(0x41c, 1 << 7 | 1 << 3);
	mchbar_setbits32(0x400, 1 << 1);
}

static void dram_powerup(const int resume)
{
	udelay(200);
	mchbar_clrsetbits32(CLKCFG_MCHBAR, 1 << 3, 3 << 21);
	if (!resume) {
		mchbar_setbits32(0x1434, 1 << 10);
		udelay(1);
	}
	mchbar_setbits32(0x1434, 1 << 6);
	if (!resume) {
		udelay(1);
		mchbar_setbits32(0x1434, 1 << 9);
		mchbar_clrbits32(0x1434, 1 << 10);
		udelay(500);
	}
}
static void dram_program_timings(const timings_t *const timings)
{
	/* Values are for DDR3. */
	const int burst_length = 8;
	const int tWTR = 4, tRTP = 1;
	int i;

	FOR_EACH_CHANNEL(i) {
		u32 reg = mchbar_read32(CxDRT0_MCHBAR(i));
		const int btb_wtp = timings->tWL + burst_length/2 + timings->tWR;
		const int btb_wtr = timings->tWL + burst_length/2 + tWTR;
		reg = (reg & ~(CxDRT0_BtB_WtP_MASK  | CxDRT0_BtB_WtR_MASK)) |
			((btb_wtp << CxDRT0_BtB_WtP_SHIFT) & CxDRT0_BtB_WtP_MASK) |
			((btb_wtr << CxDRT0_BtB_WtR_SHIFT) & CxDRT0_BtB_WtR_MASK);
		if (timings->mem_clock != MEM_CLOCK_1067MT) {
			reg = (reg & ~(0x7 << 15)) | ((9 - timings->CAS) << 15);
			reg = (reg & ~(0xf << 10)) | ((timings->CAS - 3) << 10);
		} else {
			reg = (reg & ~(0x7 << 15)) | ((10 - timings->CAS) << 15);
			reg = (reg & ~(0xf << 10)) | ((timings->CAS - 4) << 10);
		}
		reg = (reg & ~(0x7 << 5)) | (3 << 5);
		reg = (reg & ~(0x7 << 0)) | (1 << 0);
		mchbar_write32(CxDRT0_MCHBAR(i), reg);

		reg = mchbar_read32(CxDRT1_MCHBAR(i));
		reg = (reg & ~(0x03 << 28)) | ((tRTP & 0x03) << 28);
		reg = (reg & ~(0x1f << 21)) | ((timings->tRAS & 0x1f) << 21);
		reg = (reg & ~(0x07 << 10)) | (((timings->tRRD - 2) & 0x07) << 10);
		reg = (reg & ~(0x07 <<  5)) | (((timings->tRCD - 2) & 0x07) << 5);
		reg = (reg & ~(0x07 <<  0)) | (((timings->tRP - 2) & 0x07) << 0);
		mchbar_write32(CxDRT1_MCHBAR(i), reg);

		reg = mchbar_read32(CxDRT2_MCHBAR(i));
		reg = (reg & ~(0x1f << 17)) | ((timings->tFAW & 0x1f) << 17);
		if (timings->mem_clock != MEM_CLOCK_1067MT) {
			reg = (reg & ~(0x7 << 12)) | (0x2 << 12);
			reg = (reg & ~(0xf <<  6)) | (0x9 <<  6);
		} else {
			reg = (reg & ~(0x7 << 12)) | (0x3 << 12);
			reg = (reg & ~(0xf <<  6)) | (0xc <<  6);
		}
		reg = (reg & ~(0x1f << 0)) | (0x13 << 0);
		mchbar_write32(CxDRT2_MCHBAR(i), reg);

		reg = mchbar_read32(CxDRT3_MCHBAR(i));
		reg |= 0x3 << 28;
		reg = (reg & ~(0x03 << 26));
		reg = (reg & ~(0x07 << 23)) | (((timings->CAS - 3) & 0x07) << 23);
		reg = (reg & ~(0xff << 13)) | ((timings->tRFC & 0xff) << 13);
		reg = (reg & ~(0x07 <<  0)) | (((timings->tWL - 2) & 0x07) <<  0);
		mchbar_write32(CxDRT3_MCHBAR(i), reg);

		reg = mchbar_read32(CxDRT4_MCHBAR(i));
		static const u8 timings_by_clock[4][3] = {
			/*   333MHz  400MHz  533MHz
			     667MT   800MT  1067MT   */
			{     0x07,   0x0a,   0x0d   },
			{     0x3a,   0x46,   0x5d   },
			{     0x0c,   0x0e,   0x18   },
			{     0x21,   0x28,   0x35   },
		};
		const int clk_idx = 2 - timings->mem_clock;
		reg = (reg & ~(0x01f << 27)) | (timings_by_clock[0][clk_idx] << 27);
		reg = (reg & ~(0x3ff << 17)) | (timings_by_clock[1][clk_idx] << 17);
		reg = (reg & ~(0x03f << 10)) | (timings_by_clock[2][clk_idx] << 10);
		reg = (reg & ~(0x1ff <<  0)) | (timings_by_clock[3][clk_idx] <<  0);
		mchbar_write32(CxDRT4_MCHBAR(i), reg);

		reg = mchbar_read32(CxDRT5_MCHBAR(i));
		if (timings->mem_clock == MEM_CLOCK_1067MT)
			reg = (reg & ~(0xf << 28)) | (0x8 << 28);
		reg = (reg & ~(0x00f << 22)) | ((burst_length/2 + timings->CAS + 2) << 22);
		reg = (reg & ~(0x3ff << 12)) | (0x190 << 12);
		reg = (reg & ~(0x00f <<  4)) | ((timings->CAS - 2) << 4);
		reg = (reg & ~(0x003 <<  2)) | (0x001 <<  2);
		reg = (reg & ~(0x003 <<  0));
		mchbar_write32(CxDRT5_MCHBAR(i), reg);

		reg = mchbar_read32(CxDRT6_MCHBAR(i));
		reg = (reg & ~(0xffff << 16)) | (0x066a << 16); /* always 7.8us refresh rate for DDR3 */
		reg |= (1 <<  2);
		mchbar_write32(CxDRT6_MCHBAR(i), reg);
	}
}

static void dram_program_banks(const dimminfo_t *const dimms)
{
	int ch, r;

	FOR_EACH_CHANNEL(ch) {
		const int tRPALL = dimms[ch].banks == 8;

		u32 reg = mchbar_read32(CxDRT1_MCHBAR(ch)) & ~(0x01 << 15);
		IF_CHANNEL_POPULATED(dimms, ch)
			reg |= tRPALL << 15;
		mchbar_write32(CxDRT1_MCHBAR(ch), reg);

		reg = mchbar_read32(CxDRA_MCHBAR(ch)) & ~CxDRA_BANKS_MASK;
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r) {
			reg |= CxDRA_BANKS(r, dimms[ch].banks);
		}
		mchbar_write32(CxDRA_MCHBAR(ch), reg);
	}
}

static void odt_setup(const timings_t *const timings, const int sff)
{
	/* Values are for DDR3. */
	int ch;

	FOR_EACH_CHANNEL(ch) {
		u32 reg = mchbar_read32(CxODT_HIGH(ch));
		if (sff && (timings->mem_clock != MEM_CLOCK_1067MT))
			reg &= ~(0x3 << (61 - 32));
		else
			reg |= 0x3 << (61 - 32);
		reg = (reg & ~(0x3 << (52 - 32))) | (0x2 << (52 - 32));
		reg = (reg & ~(0x7 << (48 - 32))) | ((timings->CAS - 3) << (48 - 32));
		reg = (reg & ~(0xf << (44 - 32))) | (0x7 << (44 - 32));
		if (timings->mem_clock != MEM_CLOCK_1067MT) {
			reg = (reg & ~(0xf << (40 - 32))) | ((12 - timings->CAS) << (40 - 32));
			reg = (reg & ~(0xf << (36 - 32))) | (( 2 + timings->CAS) << (36 - 32));
		} else {
			reg = (reg & ~(0xf << (40 - 32))) | ((13 - timings->CAS) << (40 - 32));
			reg = (reg & ~(0xf << (36 - 32))) | (( 1 + timings->CAS) << (36 - 32));
		}
		reg = (reg & ~(0xf << (32 - 32))) | (0x7 << (32 - 32));
		mchbar_write32(CxODT_HIGH(ch), reg);

		reg = mchbar_read32(CxODT_LOW(ch));
		reg = (reg & ~(0x7 << 28)) | (0x2 << 28);
		reg = (reg & ~(0x3 << 22)) | (0x2 << 22);
		reg = (reg & ~(0x7 << 12)) | (0x2 << 12);
		reg = (reg & ~(0x7 <<  4)) | (0x2 <<  4);
		switch (timings->mem_clock) {
		case MEM_CLOCK_667MT:
			reg = (reg & ~0x7);
			break;
		case MEM_CLOCK_800MT:
			reg = (reg & ~0x7) | 0x2;
			break;
		case MEM_CLOCK_1067MT:
			reg = (reg & ~0x7) | 0x5;
			break;
		}
		mchbar_write32(CxODT_LOW(ch), reg);
	}
}

static void misc_settings(const timings_t *const timings,
			  const stepping_t stepping)
{
	mchbar_clrsetbits32(0x1260, 1 << 24 | 0x1f, timings->tRD);
	mchbar_clrsetbits32(0x1360, 1 << 24 | 0x1f, timings->tRD);

	mchbar_clrsetbits8(0x1268, 0xf, timings->tWL);
	mchbar_clrsetbits8(0x1368, 0xf, timings->tWL);
	mchbar_clrsetbits8(0x12a0, 0xf, 0xa);
	mchbar_clrsetbits8(0x13a0, 0xf, 0xa);

	mchbar_clrsetbits32(0x218, 7 << 29 | 7 << 25 | 3 << 22 | 3 << 10,
				   4 << 29 | 3 << 25 | 0 << 22 | 1 << 10);
	mchbar_clrsetbits32(0x220, 7 << 16, 1 << 21 | 1 << 16);
	mchbar_clrsetbits32(0x224, 7 << 8, 3 << 8);
	if (stepping >= STEPPING_B1)
		mchbar_setbits8(0x234, 1 << 3);
}

static void clock_crossing_setup(const fsb_clock_t fsb,
				 const mem_clock_t ddr3clock,
				 const dimminfo_t *const dimms)
{
	int ch;

	static const u32 values_from_fsb_and_mem[][3][4] = {
	/* FSB 1067MHz */{
		/* DDR3-1067 */ { 0x00000000, 0x00000000, 0x00180006, 0x00810060 },
		/* DDR3-800  */ { 0x00000000, 0x00000000, 0x0000001c, 0x000300e0 },
		/* DDR3-667  */ { 0x00000000, 0x00001c00, 0x03c00038, 0x0007e000 },
		},
	/* FSB 800MHz */{
		/* DDR3-1067 */ { 0, 0, 0, 0 },
		/* DDR3-800  */ { 0x00000000, 0x00000000, 0x0030000c, 0x000300c0 },
		/* DDR3-667  */ { 0x00000000, 0x00000380, 0x0060001c, 0x00030c00 },
		},
	/* FSB 667MHz */{
		/* DDR3-1067 */ { 0, 0, 0, 0 },
		/* DDR3-800  */ { 0, 0, 0, 0 },
		/* DDR3-667  */ { 0x00000000, 0x00000000, 0x0030000c, 0x000300c0 },
		},
	};

	const u32 *data = values_from_fsb_and_mem[fsb][ddr3clock];
	mchbar_write32(0x0208, data[3]);
	mchbar_write32(0x020c, data[2]);
	if (((fsb == FSB_CLOCK_1067MHz) || (fsb == FSB_CLOCK_800MHz)) && (ddr3clock == MEM_CLOCK_667MT))
		mchbar_write32(0x0210, data[1]);

	static const u32 from_fsb_and_mem[][3] = {
			 /* DDR3-1067    DDR3-800    DDR3-667 */
	/* FSB 1067MHz */{ 0x40100401, 0x10040220, 0x08040110, },
	/* FSB  800MHz */{ 0x00000000, 0x40100401, 0x00080201, },
	/* FSB  667MHz */{ 0x00000000, 0x00000000, 0x40100401, },
	};
	FOR_EACH_CHANNEL(ch) {
		const unsigned int mchbar = 0x1258 + (ch * 0x0100);
		if ((fsb == FSB_CLOCK_1067MHz) && (ddr3clock == MEM_CLOCK_800MT) && CHANNEL_IS_CARDF(dimms, ch))
			mchbar_write32(mchbar, 0x08040120);
		else
			mchbar_write32(mchbar, from_fsb_and_mem[fsb][ddr3clock]);
		mchbar_write32(mchbar + 4, 0);
	}
}

/* Program egress VC1 isoch timings. */
static void vc1_program_timings(const fsb_clock_t fsb)
{
	const u32 timings_by_fsb[][2] = {
	/* FSB 1067MHz */ { 0x1a, 0x01380138 },
	/* FSB  800MHz */ { 0x14, 0x00f000f0 },
	/* FSB  667MHz */ { 0x10, 0x00c000c0 },
	};
	epbar_write8(EPVC1ITC,      timings_by_fsb[fsb][0]);
	epbar_write32(EPVC1IST + 0, timings_by_fsb[fsb][1]);
	epbar_write32(EPVC1IST + 4, timings_by_fsb[fsb][1]);
}

#define DEFAULT_PCI_MMIO_SIZE 2048
#define HOST_BRIDGE	PCI_DEVFN(0, 0)

static unsigned int get_mmio_size(void)
{
	const struct device *dev;
	const struct northbridge_intel_gm45_config *cfg = NULL;

	dev = pcidev_path_on_root(HOST_BRIDGE);
	if (dev)
		cfg = dev->chip_info;

	/* If this is zero, it just means devicetree.cb didn't set it */
	if (!cfg || cfg->pci_mmio_size == 0)
		return DEFAULT_PCI_MMIO_SIZE;
	else
		return cfg->pci_mmio_size;
}

/* @prejedec if not zero, set rank size to 128MB and page size to 4KB. */
static void program_memory_map(const dimminfo_t *const dimms, const channel_mode_t mode, const int prejedec, u16 ggc)
{
	int ch, r;

	/* Program rank boundaries (CxDRBy). */
	unsigned int base = 0; /* start of next rank in MB */
	unsigned int total_mb[2] = { 0, 0 }; /* total memory per channel in MB */
	FOR_EACH_CHANNEL(ch) {
		if (mode == CHANNEL_MODE_DUAL_INTERLEAVED)
			/* In interleaved mode, start every channel from 0. */
			base = 0;
		for (r = 0; r < RANKS_PER_CHANNEL; r += 2) {
			/* Fixed capacity for pre-jedec config. */
			const unsigned int rank_capacity_mb =
				prejedec ? 128 : dimms[ch].rank_capacity_mb;
			u32 reg = 0;

			/* Program bounds in CxDRBy. */
			IF_RANK_POPULATED(dimms, ch, r) {
				base += rank_capacity_mb;
				total_mb[ch] += rank_capacity_mb;
			}
			reg |= CxDRBy_BOUND_MB(r, base);
			IF_RANK_POPULATED(dimms, ch, r+1) {
				base += rank_capacity_mb;
				total_mb[ch] += rank_capacity_mb;
			}
			reg |= CxDRBy_BOUND_MB(r+1, base);

			mchbar_write32(CxDRBy_MCHBAR(ch, r), reg);
		}
	}

	/* Program page size (CxDRA). */
	FOR_EACH_CHANNEL(ch) {
		u32 reg = mchbar_read32(CxDRA_MCHBAR(ch)) & ~CxDRA_PAGESIZE_MASK;
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r) {
			/* Fixed page size for pre-jedec config. */
			const unsigned int page_size = /* dimm page size in bytes */
				prejedec ? 4096 : dimms[ch].page_size;
			reg |= CxDRA_PAGESIZE(r, log2(page_size));
			/* deferred to f5_27: reg |= CxDRA_BANKS(r, dimms[ch].banks); */
		}
		mchbar_write32(CxDRA_MCHBAR(ch), reg);
	}

	/* Calculate memory mapping, all values in MB. */

	u32 uma_sizem = 0;
	if (!prejedec) {
		if (!(ggc & 2)) {
			printk(BIOS_DEBUG, "IGD decoded, subtracting ");

			/* Graphics memory */
			const u32 gms_sizek = decode_igd_memory_size((ggc >> 4) & 0xf);
			printk(BIOS_DEBUG, "%uM UMA", gms_sizek >> 10);

			/* GTT Graphics Stolen Memory Size (GGMS) */
			const u32 gsm_sizek = decode_igd_gtt_size((ggc >> 8) & 0xf);
			printk(BIOS_DEBUG, " and %uM GTT\n", gsm_sizek >> 10);

			uma_sizem = (gms_sizek + gsm_sizek) >> 10;
		}
		/* TSEG 2M, This amount can easily be covered by SMRR MTRR's,
		   which requires to have TSEG_BASE aligned to TSEG_SIZE. */
		pci_update_config8(PCI_DEV(0, 0, 0), D0F0_ESMRAMC, ~0x07, (1 << 1) | (1 << 0));
		uma_sizem += 2;
	}

	const unsigned int mmio_size = get_mmio_size();
	const unsigned int MMIOstart = 4096 - mmio_size + uma_sizem;
	const int me_active = pci_read_config8(PCI_DEV(0, 3, 0), PCI_CLASS_REVISION) != 0xff;
	const unsigned int ME_SIZE = prejedec || !me_active ? 0 : 32;
	const unsigned int usedMEsize = (total_mb[0] != total_mb[1]) ? ME_SIZE : 2 * ME_SIZE;
	const unsigned int claimCapable =
		!(pci_read_config32(PCI_DEV(0, 0, 0), D0F0_CAPID0 + 4) & (1 << (47 - 32)));

	const unsigned int TOM = total_mb[0] + total_mb[1];
	unsigned int TOMminusME = TOM - usedMEsize;
	unsigned int TOLUD = (TOMminusME < MMIOstart) ? TOMminusME : MMIOstart;
	unsigned int TOUUD = TOMminusME;
	unsigned int REMAPbase = 0xffff, REMAPlimit = 0;

	if (claimCapable && (TOMminusME >= (MMIOstart + 64))) {
		/* 64MB alignment: We'll lose some MBs here, if ME is on. */
		TOMminusME &= ~(64 - 1);
		/* 64MB alignment: Loss will be reclaimed. */
		TOLUD &= ~(64 - 1);
		if (TOMminusME > 4096) {
			REMAPbase = TOMminusME;
			REMAPlimit = REMAPbase + (4096 - TOLUD);
		} else {
			REMAPbase = 4096;
			REMAPlimit = REMAPbase + (TOMminusME - TOLUD);
		}
		TOUUD = REMAPlimit;
		/* REMAPlimit is an inclusive bound, all others exclusive. */
		REMAPlimit -= 64;
	}

	pci_write_config16(PCI_DEV(0, 0, 0), D0F0_TOM, (TOM >> 7) & 0x1ff);
	pci_write_config16(PCI_DEV(0, 0, 0), D0F0_TOLUD, TOLUD << 4);
	pci_write_config16(PCI_DEV(0, 0, 0), D0F0_TOUUD, TOUUD);
	pci_write_config16(PCI_DEV(0, 0, 0), D0F0_REMAPBASE, (REMAPbase >> 6) & 0x03ff);
	pci_write_config16(PCI_DEV(0, 0, 0), D0F0_REMAPLIMIT, (REMAPlimit >> 6) & 0x03ff);

	/* Program channel mode. */
	switch (mode) {
	case CHANNEL_MODE_SINGLE:
		printk(BIOS_DEBUG, "Memory configured in single-channel mode.\n");
		mchbar_clrbits32(DCC_MCHBAR, DCC_INTERLEAVED);
		break;
	case CHANNEL_MODE_DUAL_ASYNC:
		printk(BIOS_DEBUG, "Memory configured in dual-channel asymmetric mode.\n");
		mchbar_clrbits32(DCC_MCHBAR, DCC_INTERLEAVED);
		break;
	case CHANNEL_MODE_DUAL_INTERLEAVED:
		printk(BIOS_DEBUG, "Memory configured in dual-channel interleaved mode.\n");
		mchbar_clrbits32(DCC_MCHBAR, DCC_NO_CHANXOR | 1 << 9);
		mchbar_setbits32(DCC_MCHBAR, DCC_INTERLEAVED);
		break;
	}

	printk(BIOS_SPEW, "Memory map:\n"
			  "TOM   = %5uMB\n"
			  "TOLUD = %5uMB\n"
			  "TOUUD = %5uMB\n"
			  "REMAP:\t base  = %5uMB\n"
				"\t limit = %5uMB\n"
	                  "usedMEsize: %dMB\n",
			  TOM, TOLUD, TOUUD, REMAPbase, REMAPlimit, usedMEsize);
}
static void prejedec_memory_map(const dimminfo_t *const dimms, channel_mode_t mode)
{
	/* Never use dual-interleaved mode in pre-jedec config. */
	if (CHANNEL_MODE_DUAL_INTERLEAVED == mode)
		mode = CHANNEL_MODE_DUAL_ASYNC;

	program_memory_map(dimms, mode, 1, 0);
	mchbar_setbits32(DCC_MCHBAR, DCC_NO_CHANXOR);
}

static void ddr3_select_clock_mux(const mem_clock_t ddr3clock,
				  const dimminfo_t *const dimms,
				  const stepping_t stepping)
{
	const int clk1067 = (ddr3clock == MEM_CLOCK_1067MT);
	const int cardF[] = { CHANNEL_IS_CARDF(dimms, 0), CHANNEL_IS_CARDF(dimms, 1) };

	int ch;

	if (stepping < STEPPING_B1)
		die("Stepping <B1 unsupported in clock-multiplexer selection.\n");

	FOR_EACH_POPULATED_CHANNEL(dimms, ch) {
		int mixed = 0;
		if ((1 == ch) && (!CHANNEL_IS_POPULATED(dimms, 0) || (cardF[0] != cardF[1])))
			mixed = 4 << 11;
		const unsigned int b = 0x14b0 + (ch * 0x0100);
		mchbar_write32(b + 0x1c, (mchbar_read32(b + 0x1c) & ~(7 << 11)) |
					 (((             cardF[ch])?1:0) << 11) | mixed);
		mchbar_write32(b + 0x18, (mchbar_read32(b + 0x18) & ~(7 << 11)) | mixed);
		mchbar_write32(b + 0x14, (mchbar_read32(b + 0x14) & ~(7 << 11)) |
					 (((!clk1067 && !cardF[ch])?0:1) << 11) | mixed);
		mchbar_write32(b + 0x10, (mchbar_read32(b + 0x10) & ~(7 << 11)) |
					 ((( clk1067 && !cardF[ch])?1:0) << 11) | mixed);
		mchbar_write32(b + 0x0c, (mchbar_read32(b + 0x0c) & ~(7 << 11)) |
					 (((             cardF[ch])?3:2) << 11) | mixed);
		mchbar_write32(b + 0x08, (mchbar_read32(b + 0x08) & ~(7 << 11)) |
					 (2 << 11)                              | mixed);
		mchbar_write32(b + 0x04, (mchbar_read32(b + 0x04) & ~(7 << 11)) |
					 (((!clk1067 && !cardF[ch])?2:3) << 11) | mixed);
		mchbar_write32(b + 0x00, (mchbar_read32(b + 0x00) & ~(7 << 11)) |
					 ((( clk1067 && !cardF[ch])?3:2) << 11) | mixed);
	}
}
static void ddr3_write_io_init(const mem_clock_t ddr3clock,
			       const dimminfo_t *const dimms,
			       const stepping_t stepping,
			       const int sff)
{
	const int a1step = stepping >= STEPPING_CONVERSION_A1;
	const int cardF[] = { CHANNEL_IS_CARDF(dimms, 0), CHANNEL_IS_CARDF(dimms, 1) };

	int ch;

	if (stepping < STEPPING_B1)
		die("Stepping <B1 unsupported in write i/o initialization.\n");
	if (sff)
		die("SFF platform unsupported in write i/o initialization.\n");

	static const u32 ddr3_667_800_by_stepping_ddr3_and_card[][2][2][4] = {
	{ /* Stepping B3 and below */
		{ /* 667 MHz */
			{ 0xa3255008, 0x26888209, 0x26288208, 0x6188040f },
			{ 0x7524240b, 0xa5255608, 0x232b8508, 0x5528040f },
		},
		{ /* 800 MHz */
			{ 0xa6255308, 0x26888209, 0x212b7508, 0x6188040f },
			{ 0x7524240b, 0xa6255708, 0x132b7508, 0x5528040f },
		},
	},
	{ /* Conversion stepping A1 and above */
		{ /* 667 MHz */
			{ 0xc5257208, 0x26888209, 0x26288208, 0x6188040f },
			{ 0x7524240b, 0xc5257608, 0x232b8508, 0x5528040f },
		},
		{ /* 800 MHz */
			{ 0xb6256308, 0x26888209, 0x212b7508, 0x6188040f },
			{ 0x7524240b, 0xb6256708, 0x132b7508, 0x5528040f },
		}
	}};

	static const u32 ddr3_1067_by_channel_and_card[][2][4] = {
		{ /* Channel A */
			{ 0xb2254708, 0x002b7408, 0x132b8008, 0x7228060f },
			{ 0xb0255008, 0xa4254108, 0x4528b409, 0x9428230f },
		},
		{ /* Channel B */
			{ 0xa4254208, 0x022b6108, 0x132b8208, 0x9228210f },
			{ 0x6024140b, 0x92244408, 0x252ba409, 0x9328360c },
		},
	};

	FOR_EACH_POPULATED_CHANNEL(dimms, ch) {
		if ((1 == ch) && CHANNEL_IS_POPULATED(dimms, 0) && (cardF[0] == cardF[1]))
			/* Only write if second channel population differs. */
			continue;
		const u32 *const data = (ddr3clock != MEM_CLOCK_1067MT)
			? ddr3_667_800_by_stepping_ddr3_and_card[a1step][2 - ddr3clock][cardF[ch]]
			: ddr3_1067_by_channel_and_card[ch][cardF[ch]];
		mchbar_write32(CxWRTy_MCHBAR(ch, 0), data[0]);
		mchbar_write32(CxWRTy_MCHBAR(ch, 1), data[1]);
		mchbar_write32(CxWRTy_MCHBAR(ch, 2), data[2]);
		mchbar_write32(CxWRTy_MCHBAR(ch, 3), data[3]);
	}

	mchbar_write32(0x1490, 0x00e70067);
	mchbar_write32(0x1494, 0x000d8000);
	mchbar_write32(0x1590, 0x00e70067);
	mchbar_write32(0x1594, 0x000d8000);
}
static void ddr3_read_io_init(const mem_clock_t ddr3clock,
			      const dimminfo_t *const dimms,
			      const int sff)
{
	int ch;

	FOR_EACH_POPULATED_CHANNEL(dimms, ch) {
		u32 addr, tmp;
		const unsigned int base = 0x14b0 + (ch * 0x0100);
		for (addr = base + 0x1c; addr >= base; addr -= 4) {
			tmp = mchbar_read32(addr);
			tmp &= ~((3 << 25) | (1 << 8) | (7 << 16) | (0xf << 20) | (1 << 27));
			tmp |= (1 << 27);
			switch (ddr3clock) {
				case MEM_CLOCK_667MT:
					tmp |= (1 << 16) | (4 << 20);
					break;
				case MEM_CLOCK_800MT:
					tmp |= (2 << 16) | (3 << 20);
					break;
				case MEM_CLOCK_1067MT:
					if (!sff)
						tmp |= (2 << 16) | (1 << 20);
					else
						tmp |= (2 << 16) | (2 << 20);
					break;
				default:
					die("Wrong clock");
			}
			mchbar_write32(addr, tmp);
		}
	}
}

static void memory_io_init(const mem_clock_t ddr3clock,
			   const dimminfo_t *const dimms,
			   const stepping_t stepping,
			   const int sff)
{
	u32 tmp;

	if (stepping < STEPPING_B1)
		die("Stepping <B1 unsupported in "
			"system-memory i/o initialization.\n");

	tmp = mchbar_read32(0x1400);
	tmp &= ~(3<<13);
	tmp |= (1<<9) | (1<<13);
	mchbar_write32(0x1400, tmp);

	tmp = mchbar_read32(0x140c);
	tmp &= ~(0xff | (1<<11) | (1<<12) |
		 (1<<16) | (1<<18) | (1<<27) | (0xf<<28));
	tmp |= (1<<7) | (1<<11) | (1<<16);
	switch (ddr3clock) {
		case MEM_CLOCK_667MT:
			tmp |= 9 << 28;
			break;
		case MEM_CLOCK_800MT:
			tmp |= 7 << 28;
			break;
		case MEM_CLOCK_1067MT:
			tmp |= 8 << 28;
			break;
	}
	mchbar_write32(0x140c, tmp);

	mchbar_clrbits32(0x1440, 1);

	tmp = mchbar_read32(0x1414);
	tmp &= ~((1<<20) | (7<<11) | (0xf << 24) | (0xf << 16));
	tmp |= (3<<11);
	switch (ddr3clock) {
		case MEM_CLOCK_667MT:
			tmp |= (2 << 24) | (10 << 16);
			break;
		case MEM_CLOCK_800MT:
			tmp |= (3 << 24) | (7 << 16);
			break;
		case MEM_CLOCK_1067MT:
			tmp |= (4 << 24) | (4 << 16);
			break;
	}
	mchbar_write32(0x1414, tmp);

	mchbar_clrbits32(0x1418, 1 << 3 | 1 << 11 | 1 << 19 | 1 << 27);

	mchbar_clrbits32(0x141c, 1 << 3 | 1 << 11 | 1 << 19 | 1 << 27);

	mchbar_setbits32(0x1428, 1 << 14);

	tmp = mchbar_read32(0x142c);
	tmp &= ~((0xf << 8) | (0x7 << 20) | 0xf | (0xf << 24));
	tmp |= (0x3 << 20) | (5 << 24);
	switch (ddr3clock) {
		case MEM_CLOCK_667MT:
			tmp |= (2 << 8) | 0xc;
			break;
		case MEM_CLOCK_800MT:
			tmp |= (3 << 8) | 0xa;
			break;
		case MEM_CLOCK_1067MT:
			tmp |= (4 << 8) | 0x7;
			break;
	}
	mchbar_write32(0x142c, tmp);

	tmp = mchbar_read32(0x400);
	tmp &= ~((3 << 4) | (3 << 16) | (3 << 30));
	tmp |= (2 << 4) | (2 << 16);
	mchbar_write32(0x400, tmp);

	mchbar_clrbits32(0x404, 0xf << 20);

	mchbar_clrbits32(0x40c, 1 << 6);

	tmp = mchbar_read32(0x410);
	tmp &= ~(7 << 28);
	tmp |= 2 << 28;
	mchbar_write32(0x410, tmp);

	tmp = mchbar_read32(0x41c);
	tmp &= ~0x77;
	tmp |= 0x11;
	mchbar_write32(0x41c, tmp);

	ddr3_select_clock_mux(ddr3clock, dimms, stepping);

	ddr3_write_io_init(ddr3clock, dimms, stepping, sff);

	ddr3_read_io_init(ddr3clock, dimms, sff);
}

static void jedec_init(const timings_t *const timings,
		       const dimminfo_t *const dimms)
{
	if ((timings->tWR < 5) || (timings->tWR > 12))
		die("tWR value unsupported in Jedec initialization.\n");

	/* Pre-jedec settings */
	mchbar_setbits32(0x40, 1 << 1);
	mchbar_setbits32(0x230, 3 << 1);
	mchbar_setbits32(0x238, 3 << 24);
	mchbar_setbits32(0x23c, 3 << 24);

	/* Normal write pointer operation */
	mchbar_setbits32(0x14f0, 1 << 9);
	mchbar_setbits32(0x15f0, 1 << 9);

	mchbar_clrsetbits32(DCC_MCHBAR, DCC_CMD_MASK, DCC_CMD_NOP);

	pci_and_config8(PCI_DEV(0, 0, 0), 0xf0, ~(1 << 2));

	pci_or_config8(PCI_DEV(0, 0, 0), 0xf0, 1 << 2);
	udelay(2);

				  /* 5  6  7  8  9 10 11 12 */
	static const u8 wr_lut[] = { 1, 2, 3, 4, 5, 5, 6, 6 };

	const int WL = ((timings->tWL - 5) & 7) << 6;
	const int ODT_120OHMS = (1 << 9);
	const int ODS_34OHMS = (1 << 4);
	const int WR = (wr_lut[timings->tWR - 5] & 7) << 12;
	const int DLL1 = 1 << 11;
	const int CAS = ((timings->CAS - 4) & 7) << 7;
	const int INTERLEAVED = 1 << 6;/* This is READ Burst Type == interleaved. */

	int ch, r;
	FOR_EACH_POPULATED_RANK(dimms, ch, r) {
		/* We won't do this in dual-interleaved mode,
		   so don't care about the offset.
		   Mirrored ranks aren't taken into account here. */
		const u32 rankaddr = raminit_get_rank_addr(ch, r);
		printk(BIOS_DEBUG, "JEDEC init @0x%08x\n", rankaddr);
		mchbar_clrsetbits32(DCC_MCHBAR, DCC_SET_EREG_MASK, DCC_SET_EREGx(2));
		read32((u32 *)(rankaddr | WL));
		mchbar_clrsetbits32(DCC_MCHBAR, DCC_SET_EREG_MASK, DCC_SET_EREGx(3));
		read32((u32 *)rankaddr);
		mchbar_clrsetbits32(DCC_MCHBAR, DCC_SET_EREG_MASK, DCC_SET_EREGx(1));
		read32((u32 *)(rankaddr | ODT_120OHMS | ODS_34OHMS));
		mchbar_clrsetbits32(DCC_MCHBAR, DCC_CMD_MASK, DCC_SET_MREG);
		read32((u32 *)(rankaddr | WR | DLL1 | CAS | INTERLEAVED));
		mchbar_clrsetbits32(DCC_MCHBAR, DCC_CMD_MASK, DCC_SET_MREG);
		read32((u32 *)(rankaddr | WR | CAS | INTERLEAVED));
	}
}

static void ddr3_calibrate_zq(void) {
	udelay(2);

	u32 tmp = mchbar_read32(DCC_MCHBAR);
	tmp &= ~(7 << 16);
	tmp |=  (5 << 16); /* ZQ calibration mode */
	mchbar_write32(DCC_MCHBAR, tmp);

	mchbar_setbits32(CxDRT6_MCHBAR(0), 1 << 3);
	mchbar_setbits32(CxDRT6_MCHBAR(1), 1 << 3);

	udelay(1);

	mchbar_clrbits32(CxDRT6_MCHBAR(0), 1 << 3);
	mchbar_clrbits32(CxDRT6_MCHBAR(1), 1 << 3);

	mchbar_setbits32(DCC_MCHBAR, 7 << 16); /* Normal operation */
}

static void post_jedec_sequence(const int cores) {
	const int quadcore = cores == 4;

	mchbar_clrbits32(0x0040, 1 << 1);
	mchbar_clrbits32(0x0230, 3 << 1);
	mchbar_setbits32(0x0230, 1 << 15);
	mchbar_clrbits32(0x0230, 1 << 19);
	mchbar_write32(0x1250, 0x6c4);
	mchbar_write32(0x1350, 0x6c4);
	mchbar_write32(0x1254, 0x871a066d);
	mchbar_write32(0x1354, 0x871a066d);
	mchbar_setbits32(0x0238, 1 << 26);
	mchbar_clrbits32(0x0238, 3 << 24);
	mchbar_setbits32(0x0238, 1 << 23);
	mchbar_clrsetbits32(0x0238, 7 << 20, 3 << 20);
	mchbar_clrsetbits32(0x0238, 7 << 17, 6 << 17);
	mchbar_clrsetbits32(0x0238, 7 << 14, 6 << 14);
	mchbar_clrsetbits32(0x0238, 7 << 11, 6 << 11);
	mchbar_clrsetbits32(0x0238, 7 <<  8, 6 <<  8);
	mchbar_clrbits32(0x023c, 3 << 24);
	mchbar_clrbits32(0x023c, 1 << 23);
	mchbar_clrsetbits32(0x023c, 7 << 20, 3 << 20);
	mchbar_clrsetbits32(0x023c, 7 << 17, 6 << 17);
	mchbar_clrsetbits32(0x023c, 7 << 14, 6 << 14);
	mchbar_clrsetbits32(0x023c, 7 << 11, 6 << 11);
	mchbar_clrsetbits32(0x023c, 7 <<  8, 6 <<  8);

	if (quadcore) {
		mchbar_setbits32(0xb14, 0xbfbf << 16);
	}
}

static void dram_optimizations(const timings_t *const timings,
			       const dimminfo_t *const dimms)
{
	int ch;

	FOR_EACH_POPULATED_CHANNEL(dimms, ch) {
		const unsigned int mchbar = CxDRC1_MCHBAR(ch);
		u32 cxdrc1 = mchbar_read32(mchbar);
		cxdrc1 &= ~CxDRC1_SSDS_MASK;
		if (dimms[ch].ranks == 1)
			cxdrc1 |= CxDRC1_SS;
		else
			cxdrc1 |= CxDRC1_DS;
		mchbar_write32(mchbar, cxdrc1);
	}
}

u32 raminit_get_rank_addr(unsigned int channel, unsigned int rank)
{
	if (!channel && !rank)
		return 0; /* Address of first rank */

	/* Read the bound of the previous rank. */
	if (rank > 0) {
		rank--;
	} else {
		rank = 3; /* Highest rank per channel */
		channel--;
	}
	const u32 reg = mchbar_read32(CxDRBy_MCHBAR(channel, rank));
	/* Bound is in 32MB. */
	return ((reg & CxDRBy_BOUND_MASK(rank)) >> CxDRBy_BOUND_SHIFT(rank)) << 25;
}

void raminit_reset_readwrite_pointers(void)
{
	mchbar_setbits32(0x1234, 1 <<  6);
	mchbar_clrbits32(0x1234, 1 <<  6);
	mchbar_setbits32(0x1334, 1 <<  6);
	mchbar_clrbits32(0x1334, 1 <<  6);
	mchbar_clrbits32(0x14f0, 1 <<  9);
	mchbar_setbits32(0x14f0, 1 <<  9);
	mchbar_setbits32(0x14f0, 1 << 10);
	mchbar_clrbits32(0x15f0, 1 <<  9);
	mchbar_setbits32(0x15f0, 1 <<  9);
	mchbar_setbits32(0x15f0, 1 << 10);
}

void raminit(sysinfo_t *const sysinfo, const int s3resume)
{
	const dimminfo_t *const dimms = sysinfo->dimms;
	const timings_t *const timings = &sysinfo->selected_timings;

	int ch;

	timestamp_add_now(TS_INITRAM_START);

	/* Wait for some bit, maybe TXT clear. */
	if (sysinfo->txt_enabled) {
		while (!(read8((u8 *)0xfed40000) & (1 << 7))) {}
	}

	/* Collect information about DIMMs and find common settings. */
	collect_dimm_config(sysinfo);

	/* Check for bad warm boot. */
	reset_on_bad_warmboot();

	/***** From now on, program according to collected infos: *****/

	/* Program DRAM type. */
	switch (sysinfo->spd_type) {
	case DDR2:
		mchbar_setbits8(0x1434, 1 << 7);
		break;
	case DDR3:
		mchbar_setbits8(0x1434, 3 << 0);
		break;
	}

	/* Program system memory frequency. */
	set_system_memory_frequency(timings);
	/* Program IGD memory frequency. */
	set_igd_memory_frequencies(sysinfo);

	/* Configure DRAM control mode for populated channels. */
	configure_dram_control_mode(timings, dimms);

	/* Initialize RCOMP. */
	rcomp_initialization(sysinfo->stepping, sysinfo->sff);

	/* Power-up DRAM. */
	dram_powerup(s3resume);
	/* Program DRAM timings. */
	dram_program_timings(timings);
	/* Program number of banks. */
	dram_program_banks(dimms);
	/* Enable DRAM clock pairs for populated DIMMs. */
	FOR_EACH_POPULATED_CHANNEL(dimms, ch)
		mchbar_setbits32(CxDCLKDIS_MCHBAR(ch), CxDCLKDIS_ENABLE);

	/* Enable On-Die Termination. */
	odt_setup(timings, sysinfo->sff);
	/* Miscellaneous settings. */
	misc_settings(timings, sysinfo->stepping);
	/* Program clock crossing registers. */
	clock_crossing_setup(timings->fsb_clock, timings->mem_clock, dimms);
	/* Program egress VC1 timings. */
	vc1_program_timings(timings->fsb_clock);
	/* Perform system-memory i/o initialization. */
	memory_io_init(timings->mem_clock, dimms,
		       sysinfo->stepping, sysinfo->sff);

	/* Initialize memory map with dummy values of 128MB per rank with a
	   page size of 4KB. This makes the JEDEC initialization code easier. */
	prejedec_memory_map(dimms, timings->channel_mode);
	if (!s3resume)
		/* Perform JEDEC initialization of DIMMS. */
		jedec_init(timings, dimms);
	/* Some programming steps after JEDEC initialization. */
	post_jedec_sequence(sysinfo->cores);

	/* Announce normal operation, initialization completed. */
	mchbar_setbits32(DCC_MCHBAR, 0x7 << 16 | 0x1 << 19);

	pci_or_config8(PCI_DEV(0, 0, 0), 0xf0, 1 << 2);

	pci_and_config8(PCI_DEV(0, 0, 0), 0xf0, ~(1 << 2));

	/* Take a breath (the reader). */

	/* Perform ZQ calibration for DDR3. */
	if (sysinfo->spd_type == DDR3)
		ddr3_calibrate_zq();

	/* Perform receive-enable calibration. */
	raminit_receive_enable_calibration(timings, dimms);
	/* Lend clock values from receive-enable calibration. */
	mchbar_clrsetbits32(CxDRT5_MCHBAR(0), 0xf0,
		(((mchbar_read32(CxDRT3_MCHBAR(0)) >> 7) - 1) & 0xf) << 4);
	mchbar_clrsetbits32(CxDRT5_MCHBAR(1), 0xf0,
		(((mchbar_read32(CxDRT3_MCHBAR(1)) >> 7) - 1) & 0xf) << 4);

	/* Perform read/write training for high clock rate. */
	if (timings->mem_clock == MEM_CLOCK_1067MT) {
		raminit_read_training(dimms, s3resume);
		raminit_write_training(timings->mem_clock, dimms, s3resume);
	}

	igd_compute_ggc(sysinfo);

	/* Program final memory map (with real values). */
	program_memory_map(dimms, timings->channel_mode, 0, sysinfo->ggc);

	/* Some last optimizations. */
	dram_optimizations(timings, dimms);

	/* Mark raminit being finished. :-) */
	pci_and_config8(PCI_DEV(0, 0x1f, 0), 0xa2, (u8)~(1 << 7));

	raminit_thermal(sysinfo);
	init_igd(sysinfo);

	timestamp_add_now(TS_INITRAM_END);
}
