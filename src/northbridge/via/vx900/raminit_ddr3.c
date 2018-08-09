/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "early_vx900.h"
#include "raminit.h"
#include <arch/io.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <delay.h>
#include <lib.h>
#include <string.h>

/**
 * @file raminit_ddr3.c
 *
 * \brief DDR3 initialization for VIA VX900 chipset
 *
 * Rather than explain the DDR3 init algorithm, it is better to focus on what
 * works and what doesn't. Familiarity with the DDR3 spec does not hurt.
 *
 * 1 DIMMs and 2 DIMMs with one rank each works.
 * 1 rank DIMM with 2 rank DIMM works, but the odd ranks are disabled.
 * (2) 2-rank DIMMs will not work.
 *
 * It is not yet clear if odd ranks do not work because of faulty timing
 * calibration, or a misconfiguration of the MCU. I have seen this with DIMMS
 * which mirror pins on the odd rank. That could also be the issue.
 *
 * The capture window is not calibrated, but preset. Whether that preset is
 * universal or frequency dependent, and whether it is board-specific or not is
 * not yet clear. @see vx900_dram_calibrate_receive_delays().
 *
 * 4GBit and 8GBit modules may not work. This is untested. Modules with 11
 * column address bits are not tested. @see vx900_dram_map_row_col_bank()
 *
 * Everything else should be in a more or less usable state. FIXME s are placed
 * all over as a reminder that either something really needs fixing, or as a
 * reminder to double-check.
 */

/* Map BA0 <-> A17, BA1 <-> A18 */
/* Map BA2 <-> A19, RA0/RA1 must not overlap BA[0:2] */
#define VX900_MRS_MA_MAP   0x4b33	/* MA Pin Mapping for MRS commands */
#define VX900_CALIB_MA_MAP 0x5911	/* MA Pin mapping for calibrations */

/*
 * Registers 0x78 -> 0x7f contain the calibration settings for DRAM IO timing
 * The dataset in these registers is selected from 0x70.
 * Once the correct dataset is selected the delays can be altered.
 *   delay_type refers to TxDQS, TxDQ, RxDQS, or RxCR
 *   bound refers to either manual, average, upper bound, or lower bound
 */
#define CALIB_TxDQS		0
#define CALIB_TxDQ		1
#define CALIB_RxDQS		2
#define CALIB_RxDQ_CR		3

#define CALIB_AVERAGE		0
#define CALIB_LOWER		1
#define CALIB_UPPER		2
#define CALIB_MANUAL		4	/* We want this & 3 to overflow to 0 */

static void vx900_delay_calib_mode_select(u8 delay_type, u8 bound)
{
	/* Which calibration setting */
	u8 reg8 = (delay_type & 0x03) << 2;
	/* Upper, lower, average, or manual setting */
	reg8 |= (bound & 0x03);
	pci_write_config8(MCU, 0x70, reg8);
}

/*
 * The vendor BIOS does something similar to vx900_delay_calib_mode_select(),
 * then reads or write a byte, and repeats the process for all 8 bytes. This is
 * annoyingly inefficient, and we can achieve the same result in a much more
 * elegant manner.
 */
static void vx900_read_0x78_0x7f(timing_dly dly)
{
	*((u32 *) (&(dly[0]))) = pci_read_config32(MCU, 0x78);
	*((u32 *) (&(dly[4]))) = pci_read_config32(MCU, 0x7c);
}

static void vx900_write_0x78_0x7f(const timing_dly dly)
{
	pci_write_config32(MCU, 0x78, *((u32 *) (&(dly[0]))));
	pci_write_config32(MCU, 0x7c, *((u32 *) (&(dly[4]))));
}

static void vx900_read_delay_range(delay_range * d_range, u8 mode)
{
	vx900_delay_calib_mode_select(mode, CALIB_LOWER);
	vx900_read_0x78_0x7f(d_range->low);
	vx900_delay_calib_mode_select(mode, CALIB_AVERAGE);
	vx900_read_0x78_0x7f(d_range->avg);
	vx900_delay_calib_mode_select(mode, CALIB_UPPER);
	vx900_read_0x78_0x7f(d_range->high);
}

static void dump_delay(const timing_dly dly)
{
	u8 i;
	for (i = 0; i < 8; i++) {
		printram(" %.2x", dly[i]);
	}
	printram("\n");
}

static void dump_delay_range(const delay_range d_range)
{
	printram("Lower limit: ");
	dump_delay(d_range.low);
	printram("Average:     ");
	dump_delay(d_range.avg);
	printram("Upper limit: ");
	dump_delay(d_range.high);
}

/*
 * These are some "safe" values that can be used for memory initialization.
 * Some will stay untouched, and others will be overwritten later on
 */
static pci_reg8 mcu_init_config[] = {
	{0x40, 0x01},		/* Virtual rank 0 ending address = 64M - 1 */
	{0x41, 0x00}, {0x42, 0x00}, {0x43, 0x00},	/* Virtual Ranks ending */
	{0x48, 0x00},		/* Virtual rank 0 starting address = 0 */
	{0x49, 0x00}, {0x4a, 0x00}, {0x4b, 0x00},	/* Virtual Ranks beginning */
	{0x50, 0xd8},		/* Set ranks 0-3 to 11 col bits, 16 row bits */
	/* Disable all virtual ranks */
	{0x54, 0x00}, {0x55, 0x00}, {0x56, 0x00}, {0x57, 0x00},
	/* Disable rank interleaving in ranks 0-3 */
	{0x58, 0x00}, {0x59, 0x00}, {0x5a, 0x00}, {0x5b, 0x00},
	{0x6c, 0xA0},		/* Memory type: DDR3, VDIMM: 1.5V, 64-bit DRAM */
	{0xc4, 0x80},		/* Enable 8 memory banks */
	{0xc6, 0x80},		/* Minimum latency from self-refresh. Bit [7] must be 1 */
	/* FIXME: do it here or in Final config? */
	{0xc8, 0x80},		/* Enable automatic triggering of short ZQ calibration */
	{0x99, 0xf0},		/* Power Management and Bypass Reorder Queue */
	/* Enable differential DQS; MODT assertion values suggested in DS */
	{0x9e, 0xa1}, {0x9f, 0x51},
	/* DQ/DQM Duty Control - Do not put any extra delays */
	{0xe9, 0x00}, {0xea, 0x00}, {0xeb, 0x00}, {0xec, 0x00},
	{0xed, 0x00}, {0xee, 0x00}, {0xef, 0x00},
	{0xfc, 0x00}, {0xfd, 0x00}, {0xfe, 0x00}, {0xff, 0x00},
	/* The following parameters we may or may not change */
	{0x61, 0x2e},		/* DRAMC Pipeline Control */
	{0x77, 0x10},		/* MDQS Output Control */

	/* The following are parameters we'll most likely never change again */
	{0x60, 0xf4},		/* DRAM Pipeline Turn-Around Setting */
	{0x65, 0x49},		/* DRAM Arbitration Bandwidth Timer - I */
	{0x66, 0x80},		/* DRAM Queue / Arbitration */
	{0x69, 0xc6},		/* Bank Control: 8 banks, high priority refresh */
	{0x6a, 0xfc},		/* DRAMC Request Reorder Control */
	{0x6e, 0x38},		/* Burst length: 8, burst-chop: enable */
	{0x73, 0x04},		/* Close All Pages Threshold */

	/* The following need to be dynamically asserted */
	/* See: check_special_registers.c */
	{0x74, 0xa0},		/* Yes, same 0x74; add one more T */
	{0x76, 0x60},		/* Write Data Phase Control */

};

/*
 * This table keeps the driving strength control setting that we can safely use
 * during initialization. This settings come in part from SerialICE, and in part
 * from code provided by VIA.
 */
static pci_reg8 mcu_drv_ctrl_config[] = {
	{0xd3, 0x03},		/* Enable auto-compensation circuit for ODT strength */
	{0xd4, 0x80},		/* Set internal ODT to dynamically turn on or off */
	{0xd6, 0x20},		/* Enable strong driving for MA and DRAM commands */
	{0xd0, 0x88},		/* (ODT) Strength ?has effect? */
	{0xe0, 0x88},		/* DRAM Driving - Group DQS (MDQS) */
	{0xe1, 0x00},		/* Disable offset mode for driving strength control */
	{0xe2, 0x88},		/* DRAM Driving - Group DQ (MD, MDQM) */
	{0xe4, 0xcc},		/* DRAM Driving - Group CSA (MCS, MCKE, MODT) */
	{0xe8, 0x88},		/* DRAM Driving - Group MA (MA, MBA, MSRAS, MSCAS, MSWE) */
	{0xe6, 0xff},		/* DRAM Driving - Group DCLK0 (DCLK[2:0] for DIMM0) */
	{0xe7, 0xff},		/* DRAM Driving - Group DCLK1 (DCLK[5:3] for DIMM1) */
	{0xe4, 0xcc},		/* DRAM Driving - Group CSA (MCS, MCKE, MODT) */
	{0x91, 0x08},		/* MCLKO Output Phase Delay - I */
	{0x92, 0x08},		/* MCLKO Output Phase Delay - II */
	{0x93, 0x16},		/* CS/CKE Output Phase Delay */
	{0x95, 0x16},		/* SCMD/MA Output Phase Delay */
	{0x9b, 0x3f},		/* Memory Clock Output Enable */
};

static void vx900_dram_set_ma_pin_map(u16 map)
{
	pci_write_config16(MCU, 0x52, map);
}

/*
 * FIXME: This function is a complete waste of space. All we really need is a
 * MA MAP table based on either row address bits or column address bits.
 * The problem is, I do not know if this mapping is applied during the column
 * access or during the row access. At least the religiously verbose output
 * makes pretty console output.
 */
static void vx900_dram_map_pins(u8 ba0, u8 ba1, u8 ba2, u8 ra0, u8 ra1)
{
	u16 map = 0;

	printram("Mapping address pins to DRAM pins:\n");
	printram("  BA0 -> A%u\n", ba0);
	printram("  BA1 -> A%u\n", ba1);
	printram("  BA2 -> A%u\n", ba2);
	printram("  RA0 -> A%u\n", ra0);
	printram("  RA1 -> A%u\n", ra1);
	/* Make sure BA2 is enabled */
	map |= (1 << 11);

	/*
	 * Find RA1 (15:14)
	 * 00: A14
	 * 01: A16
	 * 10: A18
	 * 11: A20
	 */
	if ((ra1 & 0x01) || (ra1 < 14) || (ra1 > 20)) {
		printram("Illegal mapping RA1 -> A%u\n", ra1);
		return;
	}
	map |= (((ra1 - 14) >> 1) & 0x03) << 14;

	/*
	 * Find RA0 (13:12)
	 * 00: A15
	 * 01: A17
	 * 10: A19
	 * 11: A21
	 */
	if ((!(ra0 & 0x01)) || (ra0 < 15) || (ra0 > 21)) {
		printram("Illegal mapping RA0 -> A%u\n", ra0);
		return;
	}
	map |= (((ra0 - 15) >> 1) & 0x03) << 12;

	/*
	 * Find BA2 (10:8)
	 * x00: A14
	 * x01: A15
	 * x10: A18
	 * x11: A19
	 */
	switch (ba2) {
	case 14:
		map |= (0 << 8);
		break;
	case 15:
		map |= (1 << 8);
		break;
	case 18:
		map |= (2 << 8);
		break;
	case 19:
		map |= (3 << 8);
		break;
	default:
		printram("Illegal mapping BA2 -> A%u\n", ba2);
		break;
	}

	/*
	 * Find BA1 (6:4)
	 * 000: A12
	 * 001: A14
	 * 010: A16
	 * 011: A18
	 * 1xx: A20
	 */
	if (((ba1 & 0x01)) || (ba1 < 12) || (ba1 > 20)) {
		printram("Illegal mapping BA1 -> A%u\n", ba1);
		return;
	}
	map |= (((ba1 - 12) >> 1) & 0x07) << 4;

	/*
	 * Find BA0 (2:0)
	 * 000: A11
	 * 001: A13
	 * 010: A15
	 * 011: A17
	 * 1xx: A19
	 */
	if ((!(ba0 & 0x01)) || (ba0 < 11) || (ba0 > 19)) {
		printram("Illegal mapping BA0 -> A%u\n", ba0);
		return;
	}
	map |= (((ba0 - 11) >> 1) & 0x07) << 0;

	printram("Setting map mask (rx52) to %.4x\n", map);
	vx900_dram_set_ma_pin_map(map);
}

static void vx900_dram_write_init_config(void)
{
	/* Keep our RAM space free of legacy stuff */
	vx900_disable_legacy_rom_shadow();

	/* Now worry about the real RAM init */
	size_t i;
	for (i = 0; i < (sizeof(mcu_init_config) / sizeof(pci_reg8)); i++) {
		pci_write_config8(MCU, mcu_init_config[i].addr,
				  mcu_init_config[i].val);
	}
	vx900_dram_set_ma_pin_map(VX900_CALIB_MA_MAP);

	/* FIXME: Slowing stuff down. Does this really help? */

	/* Fast cycle control for CPU-to-DRAM Read Cycle 0:Disabled.
	 * This CPU bus controller will wait for all data */

	/* Memory to CPU bus Controller Conversion Mode 1: Synchronous  mode */
}

static void dram_find_spds_ddr3(const dimm_layout * addr, dimm_info * dimm)
{
	size_t i = 0;
	int dimms = 0;
	do {
		spd_raw_data spd;
		spd_read(addr->spd_addr[i], spd);
		spd_decode_ddr3(&dimm->dimm[i], spd);
		if (dimm->dimm[i].dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3)
			continue;
		dimms++;
		dram_print_spd_ddr3(&dimm->dimm[i]);
	} while (addr->spd_addr[++i] != SPD_END_LIST
		 && i < VX900_MAX_DIMM_SLOTS);

	if (!dimms)
		die("No DIMMs were found");
}

static void dram_find_common_params(const dimm_info * dimms,
				    ramctr_timing * ctrl)
{
	size_t i, valid_dimms;
	memset(ctrl, 0, sizeof(ramctr_timing));
	ctrl->cas_supported = 0xff;
	valid_dimms = 0;
	for (i = 0; i < VX900_MAX_DIMM_SLOTS; i++) {
		const dimm_attr *dimm = &dimms->dimm[i];
		if (dimm->dram_type == SPD_MEMORY_TYPE_UNDEFINED)
			continue;
		valid_dimms++;

		if (valid_dimms == 1) {
			/* First DIMM defines the type of DIMM */
			ctrl->dram_type = dimm->dram_type;
			ctrl->dimm_type = dimm->dimm_type;
		} else {
			/* Check if we have mismatched DIMMs */
			if (ctrl->dram_type != dimm->dram_type
				|| ctrl->dimm_type != dimm->dimm_type)
				die("Mismatched DIMM Types");
		}
		/* Find all possible CAS combinations */
		ctrl->cas_supported &= dimm->cas_supported;

		/* Find the smallest common latencies supported by all DIMMs */
		ctrl->tCK = MAX(ctrl->tCK, dimm->tCK);
		ctrl->tAA = MAX(ctrl->tAA, dimm->tAA);
		ctrl->tWR = MAX(ctrl->tWR, dimm->tWR);
		ctrl->tRCD = MAX(ctrl->tRCD, dimm->tRCD);
		ctrl->tRRD = MAX(ctrl->tRRD, dimm->tRRD);
		ctrl->tRP = MAX(ctrl->tRP, dimm->tRP);
		ctrl->tRAS = MAX(ctrl->tRAS, dimm->tRAS);
		ctrl->tRC = MAX(ctrl->tRC, dimm->tRC);
		ctrl->tRFC = MAX(ctrl->tRFC, dimm->tRFC);
		ctrl->tWTR = MAX(ctrl->tWTR, dimm->tWTR);
		ctrl->tRTP = MAX(ctrl->tRTP, dimm->tRTP);
		ctrl->tFAW = MAX(ctrl->tFAW, dimm->tFAW);

	}

	ctrl->n_dimms = valid_dimms;
	if (!ctrl->cas_supported)
		die("Unsupported DIMM combination. "
		    "DIMMS do not support common CAS latency");
	if (!valid_dimms)
		die("No valid DIMMs found");
}

static void vx900_dram_phys_bank_range(const dimm_info * dimms,
				       rank_layout * ranks)
{
	size_t i;
	for (i = 0; i < VX900_MAX_DIMM_SLOTS; i++) {
		if (dimms->dimm[i].dram_type == SPD_MEMORY_TYPE_UNDEFINED)
			continue;
		u8 nranks = dimms->dimm[i].ranks;
		/* Make sure we save the flags */
		ranks->flags[i * 2 + 1] = ranks->flags[i * 2] =
		    dimms->dimm[i].flags;
		/* Only Rank1 has a mirrored pin mapping */
		ranks->flags[i * 2].pins_mirrored = 0;
		if (nranks > 2)
			die("Found DIMM with more than two ranks, which is not "
			    "supported by this chipset");
		u32 size = dimms->dimm[i].size_mb;
		if (nranks == 2) {
			/* Each rank holds half the capacity of the DIMM */
			size >>= 1;
			ranks->phys_rank_size_mb[i << 1] = size;
			ranks->phys_rank_size_mb[(i << 1) | 1] = size;
		} else {
			/* Otherwise, everything is held in the first bank */
			ranks->phys_rank_size_mb[i << 1] = size;
			ranks->phys_rank_size_mb[(i << 1) | 1] = 0;
		}
	}
}

#define ODT_R0				0
#define ODT_R1				1
#define ODT_R2				2
#define ODT_R3				3
/*
 * This is the table that tells us which MODT pin to map to which rank.
 *
 * This table is taken from code provided by VIA, but no explanation was
 * provided as to why it is done this way. It may be possible that this table is
 * not suitable for the way we map ranks later on.
 */
static const u8 odt_lookup_table[][2] = {
	/* RankMAP    Rank 3          Rank 2          Rank 1          Rank 0 */
	{0x01, (ODT_R3 << 6) | (ODT_R2 << 4) | (ODT_R1 << 2) | (ODT_R0 << 0)},
	{0x03, (ODT_R3 << 6) | (ODT_R2 << 4) | (ODT_R0 << 2) | (ODT_R1 << 0)},
	{0x04, (ODT_R3 << 6) | (ODT_R2 << 4) | (ODT_R1 << 2) | (ODT_R0 << 0)},
	{0x05, (ODT_R3 << 6) | (ODT_R0 << 4) | (ODT_R1 << 2) | (ODT_R2 << 0)},
	{0x07, (ODT_R3 << 6) | (ODT_R0 << 4) | (ODT_R2 << 2) | (ODT_R2 << 0)},
	{0x0c, (ODT_R2 << 6) | (ODT_R3 << 4) | (ODT_R1 << 2) | (ODT_R0 << 0)},
	{0x0d, (ODT_R0 << 6) | (ODT_R0 << 4) | (ODT_R1 << 2) | (ODT_R2 << 0)},
	{0x0f, (ODT_R0 << 6) | (ODT_R0 << 4) | (ODT_R2 << 2) | (ODT_R2 << 0)},
	{0, 0},
};

static void vx900_dram_driving_ctrl(const dimm_info * dimm)
{
	size_t i, ndimms;
	u8 reg8, regxd5, rank_mask;

	rank_mask = 0;
	/* For ODT range selection, datasheet recommends
	 * when 1 DIMM  present:  60 Ohm
	 * when 2 DIMMs present: 120 Ohm  */
	ndimms = 0;
	for (i = 0; i < VX900_MAX_DIMM_SLOTS; i++) {
		if (dimm->dimm[i].dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3)
			continue;
		ndimms++;
		rank_mask |= (1 << (i * 2));
		if (dimm->dimm[i].ranks > 1)
			rank_mask |= (2 << (i * 2));
	}
	/* ODT strength and MD/MDQM/MDQS driving strength */
	if (ndimms > 1) {
		/* Enable 1 ODT block (120 Ohm ODT) */
		regxd5 = 0 << 2;
		/* Enable strong driving for MD/MDQM/MDQS */
		regxd5 |= (1 << 7);
	} else {
		/* Enable 2 ODT blocks (60 Ohm ODT) */
		regxd5 = 1 << 2;
		/* Leave MD/MDQM/MDQS driving weak */
	}
	pci_write_config8(MCU, 0xd5, regxd5);

	/* Enable strong CLK driving for DIMMs with more than one rank */
	if (dimm->dimm[0].ranks > 1)
		pci_mod_config8(MCU, 0xd6, 0, (1 << 7));
	if (dimm->dimm[1].ranks > 1)
		pci_mod_config8(MCU, 0xd6, 0, (1 << 6));

	/* DRAM ODT Lookup Table */
	for (i = 0;; i++) {
		if (odt_lookup_table[i][0] == 0) {
			printram("No ODT entry for rank mask %x\n", rank_mask);
			die("Aborting");
		}
		if (odt_lookup_table[i][0] != rank_mask)
			continue;

		reg8 = odt_lookup_table[i][1];
		break;
	}

	printram("Mapping rank mask %x to ODT entry %.2x\n", rank_mask, reg8);
	pci_write_config8(MCU, 0x9c, reg8);

	for (i = 0; i < (sizeof(mcu_drv_ctrl_config) / sizeof(pci_reg8)); i++) {
		pci_write_config8(MCU, mcu_drv_ctrl_config[i].addr,
				  mcu_drv_ctrl_config[i].val);
	}
}

static void vx900_pr_map_all_vr3(void)
{
	/* Enable all ranks and set them to VR3 */
	pci_write_config16(MCU, 0x54, 0xbbbb);
}

/* Map physical rank pr to virtual rank vr */
static void vx900_map_pr_vr(u8 pr, u8 vr)
{
	u16 val;

	pr &= 0x3;
	vr &= 0x3;
	/* Enable rank (bit [3], and set the VR number bits [1:0] */
	val = 0x8 | vr;
	/* Now move the value to the appropriate PR */
	val <<= (pr * 4);
	pci_mod_config16(MCU, 0x54, 0xf << (pr * 4), val);
	printram("Mapping PR %u to VR %u\n", pr, vr);
}

static u8 vx900_get_CWL(u8 CAS)
{
	/* Get CWL based on CAS using the following rule:
	 *       _________________________________________
	 * CAS: | 4T | 5T | 6T | 7T | 8T | 9T | 10T | 11T |
	 * CWL: | 5T | 5T | 5T | 6T | 6T | 7T |  7T |  8T |
	 */
	static const u8 cas_cwl_map[] = { 5, 5, 5, 6, 6, 7, 7, 8 };
	if (CAS > 11)
		return 8;
	return cas_cwl_map[CAS - 4];
}

/*
 * Here we are calculating latencies, and writing them to the appropriate
 * registers. Note that some registers do not take latencies from 0 = 0T,
 * 1 = 1T, so each register gets its own math formula.
 */
static void vx900_dram_timing(ramctr_timing * ctrl)
{
	u8 reg8, val, tFAW, tRRD;
	u32 val32;

	/* Maximum supported DDR3 frequency is 533MHz (DDR3 1066) so make sure
	 * we cap it if we have faster DIMMs.
	 * Then, align it to the closest JEDEC standard frequency */
	if (ctrl->tCK <= TCK_533MHZ) {
		ctrl->tCK = TCK_533MHZ;
	} else if (ctrl->tCK <= TCK_400MHZ) {
		ctrl->tCK = TCK_400MHZ;
	} else if (ctrl->tCK <= TCK_333MHZ) {
		ctrl->tCK = TCK_333MHZ;
	} else {
		ctrl->tCK = TCK_266MHZ;
	}

	val32 = (1000 << 8) / ctrl->tCK;
	printram("Selected DRAM frequency: %u MHz\n", val32);

	/* Find CAS and CWL latencies */
	val = CEIL_DIV(ctrl->tAA, ctrl->tCK);
	printram("Minimum  CAS latency   : %uT\n", val);
	/* Find lowest supported CAS latency that satisfies the minimum value */
	while (!((ctrl->cas_supported >> (val - 4)) & 1)
	       && (ctrl->cas_supported >> (val - 4))) {
		val++;
	}
	/* Is CAS supported */
	if (!(ctrl->cas_supported & (1 << (val - 4))))
		printram("CAS not supported\n");
	printram("Selected CAS latency   : %uT\n", val);
	ctrl->CAS = val;
	ctrl->CWL = vx900_get_CWL(ctrl->CAS);
	printram("Selected CWL latency   : %uT\n", ctrl->CWL);
	/* Write CAS and CWL */
	reg8 = (((ctrl->CWL - 4) & 0x07) << 4) | ((ctrl->CAS - 4) & 0x07);
	pci_write_config8(MCU, 0xc0, reg8);

	/* Find tRCD */
	val = CEIL_DIV(ctrl->tRCD, ctrl->tCK);
	printram("Selected tRCD          : %uT\n", val);
	reg8 = ((val - 4) & 0x7) << 4;
	/* Find tRP */
	val = CEIL_DIV(ctrl->tRP, ctrl->tCK);
	printram("Selected tRP           : %uT\n", val);
	reg8 |= ((val - 4) & 0x7);
	pci_write_config8(MCU, 0xc1, reg8);

	/* Find tRAS */
	val = CEIL_DIV(ctrl->tRAS, ctrl->tCK);
	printram("Selected tRAS          : %uT\n", val);
	reg8 = ((val - 15) & 0x7) << 4;
	/* Find tWR */
	ctrl->WR = CEIL_DIV(ctrl->tWR, ctrl->tCK);
	printram("Selected tWR           : %uT\n", ctrl->WR);
	reg8 |= ((ctrl->WR - 4) & 0x7);
	pci_write_config8(MCU, 0xc2, reg8);

	/* Find tFAW */
	tFAW = CEIL_DIV(ctrl->tFAW, ctrl->tCK);
	printram("Selected tFAW          : %uT\n", tFAW);
	/* Find tRRD */
	tRRD = CEIL_DIV(ctrl->tRRD, ctrl->tCK);
	printram("Selected tRRD          : %uT\n", tRRD);
	val = tFAW - 4 * tRRD;	/* number of cycles above 4*tRRD */
	reg8 = ((val - 0) & 0x7) << 4;
	reg8 |= ((tRRD - 2) & 0x7);
	pci_write_config8(MCU, 0xc3, reg8);

	/* Find tRTP */
	val = CEIL_DIV(ctrl->tRTP, ctrl->tCK);
	printram("Selected tRTP          : %uT\n", val);
	reg8 = ((val & 0x3) << 4);
	/* Find tWTR */
	val = CEIL_DIV(ctrl->tWTR, ctrl->tCK);
	printram("Selected tWTR          : %uT\n", val);
	reg8 |= ((val - 2) & 0x7);
	pci_mod_config8(MCU, 0xc4, 0x3f, reg8);

	/* DRAM Timing for All Ranks - VI
	 * [7:6] CKE Assertion Minimum Pulse Width
	 *     We probably don't want to mess with this just yet.
	 * [5:0] Refresh-to-Active or Refresh-to-Refresh (tRFC)
	 *     tRFC = (30 + 2 * [5:0])T
	 *     Since we previously set RxC4[7]
	 */
	reg8 = pci_read_config8(MCU, 0xc5);
	val = CEIL_DIV(ctrl->tRFC, ctrl->tCK);
	printram("Minimum  tRFC          : %uT\n", val);
	if (val < 30) {
		val = 0;
	} else {
		val = (val - 30 + 1) / 2;
	}
	;
	printram("Selected tRFC          : %uT\n", 30 + 2 * val);
	reg8 |= (val & 0x3f);
	pci_write_config8(MCU, 0xc5, reg8);

	/* Where does this go??? */
	val = CEIL_DIV(ctrl->tRC, ctrl->tCK);
	printram("Required tRC           : %uT\n", val);
}

/* Program the DRAM frequency */
static void vx900_dram_freq(ramctr_timing * ctrl)
{
	u8 val;

	/* Step 1 - Reset the PLL */
	pci_mod_config8(MCU, 0x90, 0x00, 0x0f);
	/* Wait at least 10 ns; VIA code delays by 640us */
	udelay(640);

	/* Step 2 - Set target frequency */
	if (ctrl->tCK <= TCK_533MHZ) {
		val = 0x07;
		ctrl->tCK = TCK_533MHZ;
	} else if (ctrl->tCK <= TCK_400MHZ) {
		val = 0x06;
		ctrl->tCK = TCK_400MHZ;
	} else if (ctrl->tCK <= TCK_333MHZ) {
		val = 0x05;
		ctrl->tCK = TCK_333MHZ;
	} else {		/*ctrl->tCK <= TCK_266MHZ */
		val = 0x04;
		ctrl->tCK = TCK_266MHZ;
	}
	/* Restart the PLL with the desired frequency */
	pci_mod_config8(MCU, 0x90, 0x0f, val);

	/* Step 3 - Wait for PLL to stabilize */
	udelay(2000);

	/* Step 4 - Reset the DLL - Clear [7,4] */
	pci_mod_config8(MCU, 0x6b, 0x90, 0x00);
	udelay(2000);

	/* Step 5 - Enable the DLL - Set bits [7,4] to 01b */
	pci_mod_config8(MCU, 0x6b, 0x00, 0x10);
	udelay(2000);

	/* Step 6 - Start DLL Calibration - Set bit [7] */
	pci_mod_config8(MCU, 0x6b, 0x00, 0x80);
	udelay(5);

	/* Step 7 - Finish DLL Calibration - Clear bit [7] */
	pci_mod_config8(MCU, 0x6b, 0x80, 0x00);

	/* Step 8 - If we have registered DIMMs, we need to set bit[0] */
	if (spd_dimm_is_registered_ddr3(ctrl->dimm_type)) {
		printram("Enabling RDIMM support in memory controller\n");
		pci_mod_config8(MCU, 0x6c, 0x00, 0x01);
	}
}

/*
 * The VX900 can send the MRS commands directly through hardware
 * It does the MR2->MR3->MR1->MR0->LongZQ JEDEC dance
 * The parameters that we don't worry about are extracted from the timing
 * registers we have programmed earlier.
 */
static void vx900_dram_ddr3_do_hw_mrs(u8 ma_swap, u8 rtt_nom,
				      u8 ods, u8 rtt_wr, u8 srt, u8 asr)
{
	u16 reg16 = 0;

	printram("Using Hardware method for DRAM MRS commands.\n");

	reg16 |= ((rtt_wr & 0x03) << 12);
	if (srt)
		reg16 |= (1 << 9);
	if (asr)
		reg16 |= (1 << 8);
	reg16 |= ((rtt_nom & 0x7) << 4);
	reg16 |= ((ods & 0x03) << 2);
	if (ma_swap)
		reg16 |= (1 << 1);
	reg16 |= (1 << 14);
	reg16 |= (1 << 0);	/* This is the trigger bit */
	printram("Hw MRS set is 0x%4x\n", reg16);
	pci_write_config16(MCU, 0xcc, reg16);
	/* Wait for MRS commands to be sent */
	while (pci_read_config8(MCU, 0xcc) & 1);
}

/*
 * Translate the MRS command into an address on the CPU bus
 *
 * Take an MRS command (mrs_cmd_t) and translate it to a read address on the CPU
 * bus. Thus, reading from the returned address, will issue the correct MRS
 * command, assuming we are in MRS mode, of course.
 *
 * A read from the returned address will produce the correct MRS command
 * provided the following conditions are met:
 *  - The MA pin mapping is set to VX900_MRS_MA_MAP
 *  - The memory controller's Fun3_RX6B[2:0] is set to 011b (MSR Enable)
 */
static u32 vx900_get_mrs_addr(mrs_cmd_t cmd)
{
	u32 addr = 0;
	u8 mrs_type = (cmd >> 16) & 0x07;
	/* MA[9:0] <-> A[12:3] */
	addr |= ((cmd & 0x3ff) << 3);
	/* MA10 <-> A20 */
	addr |= (((cmd >> 10) & 0x1) << 20);
	/* MA[12:11] <-> A[14:13] */
	addr |= (((cmd >> 11) & 0x3) << 13);
	/* BA[2:0] <-> A[19:17] */
	addr |= mrs_type << 17;
	return addr;
}

/*
 * Here, we do the MR2->MR3->MR1->MR0->LongZQ JEDEC dance manually
 *
 * Why would we do this in software, when the VX900 can do it in hardware? The
 * problem is the hardware sequence seems to be buggy on ranks with mirrored
 * pins. Is this a hardware bug or a misconfigured MCU? No idea.
 *
 * To maintain API compatibility with the function that implements the hardware
 * sequence, we don't ask for all parameters. To keep an overall cleaner code
 * structure, we don't try to pass down all that information. Instead, we
 * extract the extra parameters from the timing registers we have programmed
 * earlier.
 */
static void vx900_dram_ddr3_do_sw_mrs(u8 ma_swap, enum ddr3_mr1_rtt_nom rtt_nom,
				      enum ddr3_mr1_ods ods,
				      enum ddr3_mr2_rttwr rtt_wr,
				      enum ddr3_mr2_srt_range srt,
				      enum ddr3_mr2_asr asr)
{
	mrs_cmd_t mrs;
	u8 reg8, cas, cwl, twr;

	printram("Using Software method for DRAM MRS commands.\n");

	/* Get CAS, CWL, and tWR that we programmed earlier */
	reg8 = pci_read_config8(MCU, 0xc0);
	cas = (reg8 & 0x07) + 4;
	cwl = ((reg8 >> 4) & 0x07) + 4;
	reg8 = pci_read_config8(MCU, 0xc2);
	twr = (reg8 & 0x07) + 4;

	/* Step 06 - Set Fun3_RX6B[2:0] to 001b (NOP Command Enable). */
	/*    Was already done for us before calling us */

	/* Step 07 - Read a double word from any address of the DIMM. */
	/*    Was already done for us before calling us */

	/* Step 08 - Set Fun3_RX6B[2:0] to 011b (MSR Enable). */
	pci_mod_config8(MCU, 0x6b, 0x07, 0x03);	/* MSR Enable */

	/* Step 09 - Issue MR2 cycle. Read a double word from the address
	 * depended on DRAM's Rtt_WR and CWL settings. */
	mrs = ddr3_get_mr2(rtt_wr, srt, asr, cwl);
	if (ma_swap)
		mrs = ddr3_mrs_mirror_pins(mrs);
	volatile_read(vx900_get_mrs_addr(mrs));
	printram("MR2: %.5x\n", mrs);
	udelay(1000);

	/* Step 10 - Issue MR3 cycle. Read a double word from the address 60000h
	 * to set DRAM to normal operation mode. */
	mrs = ddr3_get_mr3(0);
	if (ma_swap)
		mrs = ddr3_mrs_mirror_pins(mrs);
	volatile_read(vx900_get_mrs_addr(mrs));
	printram("MR3: %.5x\n", mrs);
	udelay(1000);

	/* Step 11 -Issue MR1 cycle. Read a double word from the address
	 * depended on DRAM's output driver impedance and Rtt_Nom settings.
	 * The DLL enable field, TDQS field, write leveling enable field,
	 * additive latency field and Qoff field should be set to 0. */
	mrs = ddr3_get_mr1(DDR3_MR1_QOFF_ENABLE, DDR3_MR1_TQDS_DISABLE, rtt_nom,
			   DDR3_MR1_WRLVL_DISABLE, ods, DDR3_MR1_AL_DISABLE,
			   DDR3_MR1_DLL_ENABLE);
	if (ma_swap)
		mrs = ddr3_mrs_mirror_pins(mrs);
	volatile_read(vx900_get_mrs_addr(mrs));
	printram("MR1: %.5x\n", mrs);
	udelay(1000);

	/* Step 12 - Issue MR0 cycle. Read a double word from the address
	 * depended on DRAM's burst length, CAS latency and write recovery time
	 * settings.
	 * The read burst type field should be set to interleave.
	 * The mode field should be set to normal mode.
	 * The DLL reset field should be set to No.
	 * The DLL control for precharge PD field should be set to Fast exit.
	 */
	mrs = ddr3_get_mr0(DDR3_MR0_PRECHARGE_FAST, twr,
			   DDR3_MR0_DLL_RESET_NO, DDR3_MR0_MODE_NORMAL, cas,
			   DDR3_MR0_BURST_TYPE_INTERLEAVED,
			   DDR3_MR0_BURST_LENGTH_CHOP);
	volatile_read(vx900_get_mrs_addr(mrs));
	printram("MR0: %.5x\n", mrs);
	udelay(1000);

	/* Step 13 - Set Fun3_RX6B[2:0] to 110b (Long ZQ calibration cmd) */
	pci_mod_config8(MCU, 0x6b, 0x07, 0x06);	/* Long ZQ */
	/* Step 14 - Read a double word from any address of the DIMM. */
	volatile_read(0);
	udelay(1000);
}

/*
 * This is where we take the DIMMs out of reset and do the JEDEC dance for each
 * individual physical rank.
 */
static void vx900_dram_ddr3_dimm_init(const ramctr_timing * ctrl,
				      const rank_layout * ranks)
{
	size_t i;
	u8 rtt_nom, rtt_wr, ods, pinswap;

	/* Set BA[0/1/2] to [A17/18/19] */
	vx900_dram_set_ma_pin_map(VX900_MRS_MA_MAP);

	/* Step 01 - Set Fun3_Rx6E[5] to 1b to support burst length. */
	pci_mod_config8(MCU, 0x6e, 0, 1 << 5);
	/* Step 02 - Set Fun3_RX69[0] to 0b (Disable Multiple Page Mode). */
	pci_mod_config8(MCU, 0x69, (1 << 0), 0x00);
	/* And set [7:6] to 10b ? */
	pci_write_config8(MCU, 0x69, 0x87);

	/* Step 03 - Set the target physical rank to virtual rank0 and other
	 * ranks to virtual rank3. */
	vx900_pr_map_all_vr3();

	/* Step 04 - Set Fun3_Rx50 to D8h. */
	pci_write_config8(MCU, 0x50, 0xd8);
	/* Step 05 - Set Fun3_RX6B[5] to 1b to de-assert RESET# and wait for at
	 * least 500 us. */
	pci_mod_config8(MCU, 0x6b, 0x00, (1 << 5));
	udelay(500);

	/* Step 6 -> 15 - Set the target physical rank to virtual rank 0 and
	 * other ranks to virtual rank 3.
	 * Repeat Step 6 to 14 for every rank present, then jump to Step 16. */
	for (i = 0; i < VX900_MAX_MEM_RANKS; i++) {
		if (ranks->phys_rank_size_mb[i] == 0)
			continue;
		printram("Initializing rank %zu\n", i);

		/* Set target physical rank to virtual rank 0
		 * other ranks to virtual rank 3*/
		vx900_map_pr_vr(i, 0);

		/* FIXME: Is this needed on HW init? */
		pci_mod_config8(MCU, 0x6b, 0x07, 0x01);	/* Enable NOP */
		volatile_read(0x0);	/* Do NOP */
		pci_mod_config8(MCU, 0x6b, 0x07, 0x03);	/* MSR Enable */

		/* See init_dram_by_rank.c and get_basic_information.c
		 * in the VIA provided code */
		if (ctrl->n_dimms == 1) {
			rtt_nom = DDR3_MR1_RTT_NOM_RZQ2;
			rtt_wr = DDR3_MR2_RTTWR_OFF;
		} else {
			rtt_nom = DDR3_MR1_RTT_NOM_RZQ8;
			rtt_wr = DDR3_MR2_RTTWR_RZQ2;
		}
		ods = ranks->flags[i].rzq7_supported ?
		    DDR3_MR1_ODS_RZQ7 : DDR3_MR1_ODS_RZQ6;

		pinswap = (ranks->flags[i].pins_mirrored);
		if (pinswap)
			printram("Pins mirrored\n");
		printram(" Swap    : %x\n", pinswap);
		printram(" rtt_nom : %x\n", rtt_nom);
		printram(" ods     : %x\n", ods);
		printram(" rtt_wr  : %x\n", rtt_wr);
		if (RAMINIT_USE_HW_MRS_SEQ)
			vx900_dram_ddr3_do_hw_mrs(pinswap, rtt_nom, ods, rtt_wr,
						  0, 0);
		else
			vx900_dram_ddr3_do_sw_mrs(pinswap, rtt_nom, ods, rtt_wr,
						  0, 0);

		/* Normal SDRAM Mode */
		pci_mod_config8(MCU, 0x6b, 0x07, 0x00);

		/* Step 15, set the rank to virtual rank 3 */
		vx900_map_pr_vr(i, 3);
	}

	/* Step 16 - Set Fun3_Rx6B[2:0] to 000b (Normal SDRAM Mode). */
	pci_mod_config8(MCU, 0x6b, 0x07, 0x00);

	/* Set BA[0/1/2] to [A13/14/15] */
	vx900_dram_set_ma_pin_map(VX900_CALIB_MA_MAP);

	/* Step 17 - Set Fun3_Rx69[0] to 1b (Enable Multiple Page Mode). */
	pci_mod_config8(MCU, 0x69, 0x00, (1 << 0));

	printram("DIMM initialization sequence complete\n");
}

/*
 * This a small utility to send a single MRS command, but where we don't want to
 * have to worry about changing the MCU mode. It gives the MCU back to us in
 * normal operating mode.
 */
static void vx900_dram_send_soft_mrs(mrs_cmd_t cmd, u8 pin_swap)
{
	u32 addr;
	/* Set Fun3_RX6B[2:0] to 011b (MSR Enable). */
	pci_mod_config8(MCU, 0x6b, 0x07, (3 << 0));
	/* Is this a funky rank with Address pins swapped? */
	if (pin_swap)
		cmd = ddr3_mrs_mirror_pins(cmd);
	/* Find the address corresponding to the MRS */
	addr = vx900_get_mrs_addr(cmd);
	/* Execute the MRS */
	volatile_read(addr);
	/* Set Fun3_Rx6B[2:0] to 000b (Normal SDRAM Mode). */
	pci_mod_config8(MCU, 0x6b, 0x07, 0x00);
}

static void vx900_dram_enter_read_leveling(u8 pinswap)
{
	/* Precharge all before issuing read leveling MRS to DRAM */
	pci_mod_config8(MCU, 0x06b, 0x07, 0x02);
	volatile_read(0x0);
	udelay(1000);

	/* Enable read leveling: Set D0F3Rx71[7]=1 */
	pci_mod_config8(MCU, 0x71, 0, (1 << 7));

	/* Put DRAM in read leveling mode */
	mrs_cmd_t cmd = ddr3_get_mr3(1);
	vx900_dram_send_soft_mrs(cmd, pinswap);
}

static void vx900_dram_exit_read_leveling(u8 pinswap)
{
	/* Disable read leveling, and put dram in normal operation mode */
	mrs_cmd_t cmd = ddr3_get_mr3(0);
	vx900_dram_send_soft_mrs(cmd, pinswap);

	/* Disable read leveling: Set D0F3Rx71[7]=0 */
	pci_mod_config8(MCU, 0x71, (1 << 7), 0);
}

/*
 * We need to see if the delay window (difference between minimum and maximum)
 * is large enough so that we actually have a valid window. The signal should be
 * valid for at least 1/2T in general. If the window is significantly smaller,
 * then chances are our window does not latch at the correct time, and the
 * calibration will not work.
 */
#define DQSI_THRESHOLD  0x10
#define DQO_THRESHOLD   0x09
#define DQSO_THRESHOLD  0x12
#define DELAY_RANGE_GOOD   0
#define DELAY_RANGE_BAD   -1
static u8 vx900_dram_check_calib_range(const delay_range * dly, u8 window)
{
	size_t i;
	for (i = 0; i < 8; i++) {
		if (dly->high[i] - dly->low[i] < window)
			return DELAY_RANGE_BAD;
		/* When our maximum value is lower than our min, both values
		 * have overshot, and the window is definitely invalid */
		if (dly->high[i] < dly->low[i])
			return DELAY_RANGE_BAD;
	}
	return DELAY_RANGE_GOOD;
}

static void vx900_dram_find_avg_delays(vx900_delay_calib * delays)
{
	size_t i;
	u16 dq_low, dq_high, dqs_low, dqs_high, dq_final, dqs_final;
	/*
	 * At this point, we have transmit delays for both DIMMA and DIMMB, each
	 * with a slightly different window We want to find the intersection of
	 * those windows, so that we have a constrained window which both
	 * DIMMA and DIMMB can use. The center of our constrained window will
	 * also be the safest setting for the transmit delays
	 *
	 * DIMMA window t:|xxxxxxxxxxxxxx---------------xxxxxxxxxxxxxxxxxxxxxxx|
	 * DIMMB window t:|xxxxxxxxxxxxxxxxxxx---------------xxxxxxxxxxxxxxxxxx|
	 * Safe window  t:|xxxxxxxxxxxxxxxxxxx----------xxxxxxxxxxxxxxxxxxxxxxx|
	 */
	delay_range *tx_dq_a = &(delays->tx_dq[0]);
	delay_range *tx_dq_b = &(delays->tx_dq[1]);
	delay_range *tx_dqs_a = &(delays->tx_dqs[0]);
	delay_range *tx_dqs_b = &(delays->tx_dqs[1]);

	for (i = 0; i < 8; i++) {
		dq_low = MAX(tx_dq_a->low[i], tx_dq_b->low[i]);
		dq_high = MIN(tx_dq_a->high[i], tx_dq_b->high[i]);
		dqs_low = MAX(tx_dqs_a->low[i], tx_dqs_b->low[i]);
		dqs_high = MIN(tx_dqs_a->high[i], tx_dqs_b->high[i]);

		/* Find the average */
		dq_final = ((dq_low + dq_high) / 2);
		dqs_final = ((dqs_low + dqs_high) / 2);

		/*
		 * These adjustments are done in code provided by VIA.
		 * There is no explanation as to why this is done.
		 *
		 * We can get away without doing the DQS adjustment, but doing
		 * it, brings the values closer to what the vendor BIOS
		 * calibrates to.
		 */
		if ((dqs_final & 0x1f) >= 0x1c)
			dqs_final -= 0x1c;
		else
			dqs_final += 0x04;
		/*
		 * The DQ adjustment is more critical. If we don't do this
		 * adjustment our MCU won't be configured properly, and
		 * ram_check() will fail.
		 */
		if ((dq_final & 0x1f) >= 0x14)
			dq_final -= 0x14;
		else
			dq_final += 0x0c;

		/* Store our values in the first delay */
		delays->tx_dq[0].avg[i] = dq_final;
		delays->tx_dqs[0].avg[i] = dqs_final;

	}
}

/*
 * First calibration: When to receive data from the DRAM
 *	(MD and MDQS input delay)
 *
 * This calibration unfortunately does not seem to work. Whether this is due to
 * a misconfigured MCU or hardware bug is unknown.
 */
static void vx900_rx_capture_range_calib(u8 pinswap)
{
	u8 reg8;
	const u32 cal_addr = 0x20;

	/* Set IO calibration address */
	pci_mod_config16(MCU, 0x8c, 0xfff0, cal_addr & (0xfff0));
	/* Data pattern must be 0x00 for this calibration
	 * See paragraph describing Rx8e */
	pci_write_config8(MCU, 0x8e, 0x00);

	/* Need to put DRAM and MCU in read leveling */
	vx900_dram_enter_read_leveling(pinswap);

	/* Data pattern must be 0x00 for this calibration
	 * See paragraph describing Rx8e */
	pci_write_config8(MCU, 0x8e, 0x00);
	/* Trigger calibration */
	reg8 = 0xa0;
	pci_write_config8(MCU, 0x71, reg8);

	/* Wait for it */
	while (pci_read_config8(MCU, 0x71) & 0x10);
	vx900_dram_exit_read_leveling(pinswap);
}

/*
 * Second calibration: How much to delay DQS signal by
 *	(MDQS input delay)
 */
static void vx900_rx_dqs_delay_calib(u8 pinswap)
{
	const u32 cal_addr = 0x30;

	/* We need to disable refresh commands so that they don't interfere */
	const u8 ref_cnt = pci_read_config8(MCU, 0xc7);
	pci_write_config8(MCU, 0xc7, 0);
	/* Set IO calibration address */
	pci_mod_config16(MCU, 0x8c, 0xfff0, cal_addr & (0xfff0));
	/* Data pattern must be 0x00 for this calibration
	 * See paragraph describing Rx8e */
	pci_write_config8(MCU, 0x8e, 0x00);

	/* Need to put DRAM and MCU in read leveling */
	vx900_dram_enter_read_leveling(pinswap);

	/* From VIA code; Undocumented
	 * In theory this enables MODT[3:0] to be asserted */
	pci_mod_config8(MCU, 0x9e, 0, 0x80);

	/* Trigger calibration: Set D0F3Rx71[1:0]=10b */
	pci_mod_config8(MCU, 0x71, 0x03, 0x02);

	/* Wait for calibration to complete */
	while (pci_read_config8(MCU, 0x71) & 0x02);
	vx900_dram_exit_read_leveling(pinswap);

	/* Restore the refresh counter */
	pci_write_config8(MCU, 0xc7, ref_cnt);

	/* FIXME: should we save it before, or should we just set it as is */
	vx900_dram_set_ma_pin_map(VX900_CALIB_MA_MAP);
}

static void vx900_tx_dqs_trigger_calib(u8 pattern)
{
	/* Data pattern for calibration */
	pci_write_config8(MCU, 0x8e, pattern);
	/* Trigger calibration */
	pci_mod_config8(MCU, 0x75, 0, 0x20);
	/* Wait for calibration */
	while (pci_read_config8(MCU, 0x75) & 0x20);
}

/*
 * Third calibration: How much to wait before asserting DQS
 */
static void vx900_tx_dqs_delay_calib(void)
{
	const u32 cal_addr = 0x00;
	/* Set IO calibration address */
	pci_mod_config16(MCU, 0x8c, 0xfff0, cal_addr & (0xfff0));
	/* Set circuit to use calibration results - Clear Rx75[0] */
	pci_mod_config8(MCU, 0x75, 0x01, 0);
	/* Run calibration with first data pattern */
	vx900_tx_dqs_trigger_calib(0x5a);
	/* Run again with different pattern */
	vx900_tx_dqs_trigger_calib(0xa5);
}

/*
 * Fourt calibration: How much to wait before putting data on DQ lines
 */
static void vx900_tx_dq_delay_calib(void)
{
	/* Data pattern for calibration */
	pci_write_config8(MCU, 0x8e, 0x5a);
	/* Trigger calibration */
	pci_mod_config8(MCU, 0x75, 0, 0x02);
	/* Wait for calibration */
	while (pci_read_config8(MCU, 0x75) & 0x02);
}

static void vx900_rxdqs_adjust(delay_range * dly)
{
	/* Adjust Rx DQS delay after calibration has been run. This is
	 * recommended by VIA, but no explanation was provided as to why */
	size_t i;
	for (i = 0; i < 8; i++) {
		if (dly->low[i] < 3) {
			if (i == 2 || i == 4)
				dly->avg[i] += 4;
			else
				dly->avg[i] += 3;

		}

		if (dly->high[i] > 0x38)
			dly->avg[i] -= 6;
		else if (dly->high[i] > 0x30)
			dly->avg[i] -= 4;

		if (dly->avg[i] > 0x20)
			dly->avg[i] = 0x20;
	}

	/* Put Rx DQS delay into manual mode (Set Rx[2,0] to 01) */
	pci_mod_config8(MCU, 0x71, 0x05, 0x01);
	/* Now write the new settings */
	vx900_delay_calib_mode_select(CALIB_RxDQS, CALIB_MANUAL);
	vx900_write_0x78_0x7f(dly->avg);
}

static void vx900_dram_calibrate_receive_delays(vx900_delay_calib * delays,
						u8 pinswap)
{
	size_t n_tries = 0;
	delay_range *rx_dq_cr = &(delays->rx_dq_cr);
	delay_range *rx_dqs = &(delays->rx_dqs);
	/* We really should be able to finish this in a single pass, but it may
	 * in very rare circumstances not work the first time. We define a limit
	 * on the number of tries so that we have a way of warning the user */
	const size_t max_tries = 100;
	for (;;) {
		if (n_tries++ >= max_tries) {
			die("Could not calibrate receive delays. Giving up");
		}
		u8 result;
		/* Run calibrations */
		if (RAMINIT_USE_HW_RXCR_CALIB) {
			vx900_rx_capture_range_calib(pinswap);
			vx900_read_delay_range(rx_dq_cr, CALIB_RxDQ_CR);
			dump_delay_range(*rx_dq_cr);

		} else {
			/*FIXME: Cheating with Rx CR setting\
			 * We need to either use Rx CR calibration
			 * or set up a table for the calibration */
			u8 *override = &(rx_dq_cr->avg[0]);
			override[0] = 0x28;
			override[1] = 0x1c;
			override[2] = 0x28;
			override[3] = 0x28;
			override[4] = 0x2c;
			override[5] = 0x30;
			override[6] = 0x30;
			override[7] = 0x34;
			printram("Bypassing RxCR 78-7f calibration with:\n");
			dump_delay(rx_dq_cr->avg);
		}
		/* We need to put the setting on manual mode */
		pci_mod_config8(MCU, 0x71, 0, 1 << 4);
		vx900_delay_calib_mode_select(CALIB_RxDQ_CR, CALIB_MANUAL);
		vx900_write_0x78_0x7f(rx_dq_cr->avg);

		/************* RxDQS *************/
		vx900_rx_dqs_delay_calib(pinswap);
		vx900_read_delay_range(rx_dqs, CALIB_RxDQS);
		vx900_rxdqs_adjust(rx_dqs);

		result = vx900_dram_check_calib_range(rx_dqs, DQSI_THRESHOLD);
		if (result != DELAY_RANGE_GOOD)
			continue;

		/* We're good to go. Switch to manual and write the manual
		 * setting */
		pci_mod_config8(MCU, 0x71, 0, 1 << 0);
		vx900_delay_calib_mode_select(CALIB_RxDQS, CALIB_MANUAL);
		vx900_write_0x78_0x7f(rx_dqs->avg);
		break;
	}
	if (n_tries > 1)
		printram("Hmm, we had to try %zu times before our calibration "
			 "was good.\n", n_tries);
}

static void vx900_dram_calibrate_transmit_delays(delay_range * tx_dq,
						 delay_range * tx_dqs)
{
	/* Same timeout reasoning as in receive delays */
	size_t n_tries = 0;
	int dq_tries = 0, dqs_tries = 0;
	const size_t max_tries = 100;
	for (;;) {
		if (n_tries++ >= max_tries) {
			printram("Tried DQS %i times and DQ %i times\n",
				 dqs_tries, dq_tries);
			printram("Tx DQS calibration results\n");
			dump_delay_range(*tx_dqs);
			printram("TX DQ delay calibration results:\n");
			dump_delay_range(*tx_dq);
			die("Could not calibrate transmit delays. Giving up");
		}
		u8 result;
		/************* TxDQS *************/
		dqs_tries++;
		vx900_tx_dqs_delay_calib();
		vx900_read_delay_range(tx_dqs, CALIB_TxDQS);

		result = vx900_dram_check_calib_range(tx_dqs, DQSO_THRESHOLD);
		if (result != DELAY_RANGE_GOOD)
			continue;

		/************* TxDQ  *************/
		/* FIXME: not sure if multiple page mode should be enabled here
		 * Vendor BIOS does it */
		pci_mod_config8(MCU, 0x69, 0, 0x01);

		dq_tries++;
		vx900_tx_dq_delay_calib();
		vx900_read_delay_range(tx_dq, CALIB_TxDQ);

		result = vx900_dram_check_calib_range(tx_dq, DQO_THRESHOLD);
		if (result != DELAY_RANGE_GOOD)
			continue;

		/* At this point, our RAM should give correct read-backs for
		 * addresses under 64 MB. If it doesn't, it won't work */
		if (ram_check_noprint_nodie(1 << 20, 1 << 20)) {
			/* No, our RAM is not working, try again */
			/* FIXME: Except that we have not yet told the MCU what
			 * the geometry of the DIMM is, hence we don't trust
			 * this test for now */
		}
		/* Good. We should be able to use this DIMM */
		/* That's it. We're done */
		break;
	}
	if (n_tries > 1)
		printram("Hmm, we had to try %zu times before our calibration "
			 "was good.\n", n_tries);
}

/*
 * The meat and potatoes of getting our MCU to operate the DIMMs properly.
 *
 * Thank you JEDEC for making us need configurable delays for each set of MD
 * signals.
 */
static void vx900_dram_calibrate_delays(const ramctr_timing * ctrl,
					const rank_layout * ranks)
{
	size_t i;
	u8 val;
	u8 dimm;
	vx900_delay_calib delay_cal;
	memset(&delay_cal, 0, sizeof(delay_cal));
	printram("Starting delay calibration\n");

	/**** Read delay control ****/
	/* MD Input Data Push Timing Control;
	 *     use values recommended in datasheet
	 * Setting this too low causes the Rx window to move below the range we
	 * need it so we can capture it with Rx_78_7f
	 * This causes Rx calibrations to be too close to 0, and Tx
	 * calibrations will fail.
	 * Setting this too high causes the window to move above the range.
	 */
	if (ctrl->tCK <= TCK_533MHZ)
		val = 2;
	else if (ctrl->tCK <= TCK_333MHZ)
		val = 1;
	else
		val = 0;
	val++;			/* FIXME: vendor BIOS sets this to 3 */
	pci_mod_config8(MCU, 0x74, (0x03 << 1), ((val & 0x03) << 1));

	/* FIXME: The vendor BIOS increases the MD input delay - WHY ? */
	pci_mod_config8(MCU, 0xef, (3 << 4), 3 << 4);

	/**** Write delay control ****/
	/* FIXME: The vendor BIOS does this, but WHY?
	 * See check_special_registers in VIA provided code. This value seems
	 * to depend on the DRAM frequency.
	 */
	/* Early DQ/DQS for write cycles */
	pci_mod_config8(MCU, 0x76, (3 << 2), 2 << 2);
	/* FIXME: The vendor BIOS does this - Output preamble ? */
	pci_write_config8(MCU, 0x77, 0x10);

	/* Set BA[0/1/2] to [A17/18/19] */
	vx900_dram_set_ma_pin_map(VX900_MRS_MA_MAP);
	/* Disable Multiple Page Mode - Set Rx69[0] to 0 */
	pci_mod_config8(MCU, 0x69, (1 << 0), 0x00);

	/* It's very important that we keep all ranks which are not calibrated
	 * mapped to VR3. Even if we disable them, if they are mapped to VR0
	 * (the rank we use for calibrations), the calibrations may fail in
	 * unexpected ways. */
	vx900_pr_map_all_vr3();

	/* We only really need to run the receive calibrations once. They are
	 * meant to account for signal travel differences in the internal paths
	 * of the MCU, so it doesn't really matter which rank we use for this.
	 * Differences between ranks will be accounted for in the transmit
	 * calibration. */
	for (i = 0; i < VX900_MAX_DIMM_SLOTS; i += 2) {
		/* Do we have a valid DIMM? */
		if (ranks->phys_rank_size_mb[i] +
		    ranks->phys_rank_size_mb[i + 1] == 0)
			continue;
		/* Map the first rank of the DIMM to VR0 */
		vx900_map_pr_vr(2 * i, 0);
		/* Only run on first rank, remember? */
		break;
	}
	vx900_dram_calibrate_receive_delays(&delay_cal,
					    ranks->flags[i].pins_mirrored);
	printram("RX DQS calibration results\n");
	dump_delay_range(delay_cal.rx_dqs);

	/* Enable multiple page mode for when calibrating transmit delays */
	pci_mod_config8(MCU, 0x69, 0, 1 << 1);

	/*
	 * Unlike the receive delays, we need to run the transmit calibration
	 * for each DIMM (not rank). We run the calibration on the even rank.
	 * The odd rank may have memory pins swapped, and this, it seems,
	 * confuses the calibration circuit.
	 */
	dimm = 0;
	for (i = 0; i < VX900_MAX_DIMM_SLOTS; i++) {
		/* Do we have a valid DIMM? */
		u32 dimm_size_mb = ranks->phys_rank_size_mb[2 * i]
		    + ranks->phys_rank_size_mb[2 * i + 1];
		if (dimm_size_mb == 0)
			continue;
		/* Map the first rank of the DIMM to VR0 */
		vx900_map_pr_vr(2 * i, 0);
		vx900_dram_calibrate_transmit_delays(&(delay_cal.tx_dq[dimm]),
						     &(delay_cal.tx_dqs[dimm]));
		/* We run this more than once, so dump delays for each DIMM */
		printram("Tx DQS calibration results\n");
		dump_delay_range(delay_cal.tx_dqs[dimm]);
		printram("TX DQ delay calibration results:\n");
		dump_delay_range(delay_cal.tx_dq[dimm]);
		/* Now move the DIMM back to VR3 */
		vx900_map_pr_vr(2 * i, 3);
		/* We use dimm as a counter so that we fill tx_dq[] and tx_dqs[]
		 * results in order from 0, and do not leave any gaps */
		dimm++;
	}

	/* When we have more dimms, we need to find a tx window with which all
	 * dimms can safely work */
	if (dimm > 1) {
		vx900_dram_find_avg_delays(&delay_cal);
		printram("Final delay values\n");
		printram("Tx DQS:      ");
		dump_delay(delay_cal.tx_dqs[0].avg);
		printram("Tx DQ:       ");
		dump_delay(delay_cal.tx_dq[0].avg);
	}
	/* Write manual settings */
	pci_mod_config8(MCU, 0x75, 0, 0x01);
	vx900_delay_calib_mode_select(CALIB_TxDQS, CALIB_MANUAL);
	vx900_write_0x78_0x7f(delay_cal.tx_dqs[0].avg);
	vx900_delay_calib_mode_select(CALIB_TxDQ, CALIB_MANUAL);
	vx900_write_0x78_0x7f(delay_cal.tx_dq[0].avg);
}

static void vx900_dram_set_refresh_counter(ramctr_timing * ctrl)
{
	u8 reg8;
	/* Set DRAM refresh counter
	 * Based on a refresh counter of 0x61 at 400MHz */
	reg8 = (TCK_400MHZ * 0x61) / ctrl->tCK;
	pci_write_config8(MCU, 0xc7, reg8);
}

/*
 * Here, we map each rank somewhere in our address space. We don't really care
 * at this point if this will overlap the PCI config space. If needed, remapping
 * is done in ramstage, where we actually know how much PCI space we actually
 * need.
 */
static void vx900_dram_range(ramctr_timing * ctrl, rank_layout * ranks)
{
	size_t i, vrank = 0;
	u8 reg8;
	u32 ramsize_mb = 0, tolm_mb;
	const u32 TOLM_3_5G = (7 << 29);
	/* All unused physical ranks go to VR3. Otherwise, the MCU might be
	 * trying to read or write from unused ranks, or even worse, write some
	 * bits to the rank we want, and some to the unused ranks, even though
	 * they are disabled. Since VR3 is the last virtual rank to be used, we
	 * eliminate any ambiguities that the MCU may face. */
	vx900_pr_map_all_vr3();
	for (i = 0; i < VX900_MAX_MEM_RANKS; i++) {
		u32 rank_size_mb = ranks->phys_rank_size_mb[i];
		if (!rank_size_mb)
			continue;

		/* vvvvvvvvvv FIXME: Fix odd rank init vvvvvvvvvv */
		if ((i & 1)) {
			printk(BIOS_EMERG, "I cannot initialize rank %zu\n", i);
			printk(BIOS_EMERG, "I have to disable it\n");
			continue;
		}
		/* ^^^^^^^^^^ FIXME: Fix odd rank init ^^^^^^^^^^ */

		ranks->virt[vrank].start_addr = ramsize_mb;
		ramsize_mb += rank_size_mb;
		ranks->virt[vrank].end_addr = ramsize_mb;

		/* Rank memory range */
		reg8 = (ranks->virt[vrank].start_addr >> 6);
		pci_write_config8(MCU, 0x48 + vrank, reg8);
		reg8 = (ranks->virt[vrank].end_addr >> 6);
		pci_write_config8(MCU, 0x40 + vrank, reg8);

		vx900_map_pr_vr(i, vrank);

		printram("Mapped Physical rank %u, to virtual rank %u\n"
			 "    Start address: 0x%.10llx\n"
			 "    End   address: 0x%.10llx\n",
			 (int)i, (int)vrank,
			 (u64) ranks->virt[vrank].start_addr << 20,
			 (u64) ranks->virt[vrank].end_addr << 20);
		/* Move on to next virtual rank */
		vrank++;
	}

	/* Limit the Top of Low memory at 3.5G
	 * Not to worry, we'll set tolm in ramstage, once we have initialized
	 * all devices and know pci_tolm. */
	tolm_mb = MIN(ramsize_mb, TOLM_3_5G >> 20);
	u16 reg_tolm = (tolm_mb << 4) & 0xfff0;
	pci_mod_config16(MCU, 0x84, 0xfff0, reg_tolm);

	printram("Initialized %u virtual ranks, with a total size of %u MB\n",
		 (int)vrank, ramsize_mb);
}

/*
 * Here, we tell the memory controller how to treat a DIMM. This is an extremely
 * important step. It tells the MCU how many address bits we have in each DIMM,
 * and how to use them. This information is essential for the controller to
 * understand the DIMM addressing, and write and read data in the correct place.
 */
static void vx900_dram_map_row_col_bank(dimm_info * dimms)
{
	u8 reg8, rcb_val, col_bits, max_row_bits;
	size_t i;
	/* Do we have 4Gbit chips? */
	/* FIXME: Implement this */

	/* Do we have 8Gbit chips? */
	/* FIXME: Implement this */

	max_row_bits = rcb_val = reg8 = 0;
	for (i = 0; i < VX900_MAX_DIMM_SLOTS; i++) {
		if (dimms->dimm[i].dram_type == SPD_MEMORY_TYPE_UNDEFINED)
			continue;

		col_bits = dimms->dimm[i].col_bits;

		/*
		 * DDR3 always uses 3 bank address bits, and MA type 111b cannot
		 * be used due to chipset limitation. We are left with only two
		 * options, which we can choose based solely on the number of
		 * column address bits.
		 */
		if ((col_bits < 10) || (col_bits > 11)) {
			printram("DIMM %zd has %d column address bits.\n",
				 i, col_bits);
			die("Unsupported DIMM. Try booting without this DIMM");
		}

		rcb_val = col_bits - 5;
		reg8 |= (rcb_val << ((i * 3) + 2));

		/* */
		max_row_bits = MAX(max_row_bits, dimms->dimm[i].row_bits);
	}

	printram("RCBA map (rx50) <- %.2x\n", reg8);
	pci_write_config8(MCU, 0x50, reg8);

	printram("Houston, we have %d row address bits\n", max_row_bits);
	/* FIXME: Do this properly */
	vx900_dram_map_pins(13, 14, 15, 17, 16);

}

/*
 * Here, we set some final configuration bits, which should improve the
 * performance of the memory slightly (arbitration, expiration counters, etc.)
 *
 * FIXME: We don't really do much else than the minimum to get the MCU properly
 * configured. We don't yet do set the "performance-enhancing" bits referenced
 * in the comment above.
 */
static void vx900_dram_write_final_config(ramctr_timing * ctrl)
{
	/* FIXME: These are quick cheats */

	/* FIXME: Why are we doing this? */
	/* Tri-state  MCSi# when rank is in self-refresh */
	pci_mod_config8(MCU, 0x99, 0, 0x0f);

	/* Enable paging mode and 8 page registers */
	pci_mod_config8(MCU, 0x69, 0, 0xe5);

	/* Enable automatic triggering of short ZQ calibration */
	pci_write_config8(MCU, 0xc8, 0x80);

	/* And last but not least, Enable A20 line */
	outb(inb(0x92) | (1 << 1), 0x92);
}

void vx900_init_dram_ddr3(const dimm_layout * dimm_addr)
{
	dimm_info dimm_prop;
	ramctr_timing ctrl_prop;
	rank_layout ranks;
	pci_devfn_t mcu;

	if (!ram_check_noprint_nodie(1 << 20, 1 << 20)) {
		printram("RAM is already initialized. Skipping init\n");
		return;
	}
	/* Locate the Memory controller */
	mcu = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_MEMCTRL), 0);

	if (mcu == PCI_DEV_INVALID) {
		die("Memory Controller not found\n");
	}
	memset(&dimm_prop, 0, sizeof(dimm_prop));
	memset(&ctrl_prop, 0, sizeof(ctrl_prop));
	memset(&ranks, 0, sizeof(ranks));
	/* 1) Write some initial "safe" parameters */
	vx900_dram_write_init_config();
	/* 2) Get timing information from SPDs */
	dram_find_spds_ddr3(dimm_addr, &dimm_prop);
	/* 3) Find lowest common denominator for all modules */
	dram_find_common_params(&dimm_prop, &ctrl_prop);
	/* 4) Find the size of each memory rank */
	vx900_dram_phys_bank_range(&dimm_prop, &ranks);
	/* 5) Set DRAM driving strength */
	vx900_dram_driving_ctrl(&dimm_prop);
	/* 6) Set DRAM frequency and latencies */
	vx900_dram_timing(&ctrl_prop);
	vx900_dram_freq(&ctrl_prop);
	/* 7) Initialize the modules themselves */
	vx900_dram_ddr3_dimm_init(&ctrl_prop, &ranks);
	/* 8) Set refresh counter based on DRAM frequency */
	vx900_dram_set_refresh_counter(&ctrl_prop);
	/* 9) Calibrate receive and transmit delays */
	vx900_dram_calibrate_delays(&ctrl_prop, &ranks);
	/* 10) Enable Physical to Virtual Rank mapping */
	vx900_dram_range(&ctrl_prop, &ranks);
	/* 11) Map address bits to DRAM pins */
	vx900_dram_map_row_col_bank(&dimm_prop);
	/* 99) Some final adjustments */
	vx900_dram_write_final_config(&ctrl_prop);
	/* Take a dump */
	dump_pci_device(mcu);
}
